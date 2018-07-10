module Connection = {
  module Events = {
    [@bs.deriving jsConverter]
    type t = [
      | `connecting
      | `connected
      | [@bs.as "open"] `_open
      | `disconnecting
      | `disconnected
      | `close
      | `reconnected
      | `error
      | `fullsetup
      | `all
    ];
  };

  type t;

  [@bs.module "mongoose"] external connect : string => unit = "";

  [@bs.module "mongoose"] external connection : t = "";

  [@bs.send] external externalOn : (t, string, unit => unit) => unit = "on";

  let on = (event: Events.t, callback: unit => unit) =>
    externalOn(connection, event |. Events.tToJs, callback);
};

module SchemaType = {
  module JsType = {
    type t =
      | String
      | Int
      | Float
      | Boolean
      | Date;

    let toRawType = (typ: t) =>
      switch (typ) {
      | String => %bs.raw
                  {|String|}
      | Int
      | Float => %bs.raw
                 {|Number|}
      | Boolean => %bs.raw
                   {|Boolean|}
      | Date => %bs.raw
                {|Date|}
      };
  };

  [@bs.deriving abstract]
  type t = {
    [@bs.as "type"]
    _type: JsType.t,
    [@bs.optional]
    required: bool,
  };
  let make = t;

  external toObj : t => Js.t({..}) = "%identity";
  external toT : Js.t({..}) => t = "%identity";

  let convert = (sType: t) =>
    Js.Obj.assign(
      sType |. toObj,
      {"type": sType |. _typeGet |. JsType.toRawType},
    )
    |. toT;
};

module type SchemaDefinition = {
  type t;
  type def;
  type defJs;
  type defValues;
  let defToJs: def => defJs;
  let def: def;

  let modelName: string;

  type instanceType;
};

module MakeSchema = (SType: SchemaDefinition) => {
  let defJs = SType.def |. SType.defToJs;

  external toObj : 'a => Js.t({..}) = "%identity";
  external toDict : 'a => Js.Dict.t(SchemaType.t) = "%identity";
  external toDefJs : Js.t({..}) => SType.defJs = "%identity";
  let () =
    defJs
    |. toObj
    |. Js.Obj.keys
    |. Belt.Array.forEach(key =>
         defJs
         |. toDict
         |. Js.Dict.set(
              key,
              SchemaType.convert(
                defJs |. toDict |. Js.Dict.get(key) |. Belt.Option.getExn,
              ),
            )
       );

  [@bs.module "mongoose"] [@bs.new]
  external create : SType.defJs => SType.t = "Schema";

  let schema = create(defJs);
};

module ModelInstance = (SType: SchemaDefinition) => {
  type t = SType.instanceType;

  [@bs.get_index] external get : (t, string) => 'a = "";
  let _id: t => string = (i: t) => i |. get("_id");
};

module ModelQuery = {
  type t;

  type polyType = [
    | `Int(int)
    | `String(string)
    | `Float(float)
    | `Bool(bool)
    | `Rule(rule)
  ]
  and rule =
    | Declaration(string, polyType)
    | Selector(string, list(rule));

  external castToT : 'a => t = "%identity";
  external castToUnsafe : 'a => 'b = "%identity";

  let build = (rules: list(rule)) => {
    let rec innerBuild = rules => {
      let rec toJs = rule =>
        switch (rule) {
        | Declaration(name, value) => (
            name,
            switch (value) {
            | `Int(v) => Js.Json.number(v |. float_of_int) |. castToUnsafe
            | `String(v) => Js.Json.string(v) |. castToUnsafe
            | `Float(v) => Js.Json.number(v) |. castToUnsafe
            | `Bool(v) => Js.Json.boolean(v) |. castToUnsafe
            | `Rule(r) => innerBuild([r]) |. castToUnsafe
            },
          )
        | Selector(name, rules) => (
            name,
            rules
            |. Belt.List.map(rule => innerBuild([rule]))
            |. Belt.List.toArray
            |. Js.Json.array,
          )
        };
      rules |. Belt.List.map(toJs) |. Js.Dict.fromList |. Js.Json.object_;
    };
    Js.log(rules |. innerBuild);
    rules |. innerBuild |. castToT;
  };

  let d = (key, value) => Declaration(key, value);

  /** Top Level Operators */
  let or_ = (subFields: list(rule)) => Selector("$or", subFields);
  let nor = (subFields: list(rule)) => Selector("$nor", subFields);

  /** Lower Operators */
  let gt = (gt: float) => `Rule(Declaration("$gt", `Float(gt)));

  /** Default fields */
  let _id = (id: string) => Declaration("_id", `String(id));
};

module MakeModel = (SType: SchemaDefinition) => {
  type t;
  [@bs.module "mongoose"]
  external createExternal : (string, SType.t) => t = "model";

  let setup = (schema: SType.t) => createExternal(SType.modelName, schema);

  [@bs.send]
  external create : (t, SType.defValues) => Js.Promise.t(SType.instanceType) =
    "";
  [@bs.send]
  external find :
    (t, ModelQuery.t) => Js.Promise.t(array(SType.instanceType)) =
    "";
  [@bs.send]
  external findOne :
    (t, ModelQuery.t) => Js.Promise.t(Js.Nullable.t(SType.instanceType)) =
    "";
};