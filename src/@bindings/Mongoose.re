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
      | Boolean
      | Date;

    let toRawType = (typ: t) =>
      switch (typ) {
      | String => %bs.raw
                  {|String|}
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
      {"type": sType |. _type |. JsType.toRawType},
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

module MakeModel = (SType: SchemaDefinition) => {
  type t;
  [@bs.module "mongoose"]
  external createExternal : (string, SType.t) => t = "model";

  let setup = (schema: SType.t) => createExternal(SType.modelName, schema);

  [@bs.send] external create : (t, SType.defValues) => Js.Promise.t(t) = "";
};
