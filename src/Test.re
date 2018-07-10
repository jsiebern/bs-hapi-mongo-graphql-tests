/* let server =
     Hapi.Server.make(
       Hapi.Server.ServerOptions.make(~host="localhost", ~port=6060, ()),
     );
   server
   |. Hapi.Server.Instance.start
   |. Future.get(v => {
        Js.log("OWWWWWW");
        let info = server |. Hapi.Server.Instance.info;

        Hapi.Server.Instance.Info.(Js.log(info));
      });

   let info = server |. Hapi.Server.Instance.info;

   Hapi.Server.Instance.Info.(Js.log(info)); */

Mongoose.Connection.connect("mongodb://127.0.0.1:27017");
Mongoose.Connection.on(`connecting, () => Js.log("connecting to MongoDB..."));
Mongoose.Connection.on(`connected, () => Js.log("MongoDB connected!"));

module KeyModel = {
  module SchemaDef = {
    type t;
    [@bs.deriving jsConverter]
    type def = {
      key: Mongoose.SchemaType.t,
      num: Mongoose.SchemaType.t,
    };
    type defJs = {
      .
      "key": Mongoose.SchemaType.t,
      "num": Mongoose.SchemaType.t,
    };
    type defValues = {
      .
      "key": string,
      "num": int,
    };
    let def = {
      key: Mongoose.SchemaType.make(~_type=String, ~required=true, ()),
      num: Mongoose.SchemaType.make(~_type=Int, ()),
    };
    let modelName = "KeyModel";

    type instanceType;
  };

  module Schema = Mongoose.MakeSchema(SchemaDef);
  module Model = Mongoose.MakeModel(SchemaDef);

  module Instance = {
    include Mongoose.ModelInstance(SchemaDef);

    let key: t => string = (i: t) => i |. get("key");
    let num: t => int = (i: t) => i |. get("num") |. int_of_float;
  };

  module Query = {
    include Mongoose.ModelQuery;

    let key = (value: [> | `String(string) | `Rule(rule)]) =>
      d("key", value);
    let num = (value: int) => d("num", `Float(value |. float_of_int));
  };

  let schema = Schema.schema;
  let model = Model.setup(schema);
};

FutureJs.fromPromise(
  KeyModel.model
  |. KeyModel.Model.findOne(
       KeyModel.Query.(
         build([
           and_([num(2) |. gt, path("key.something", `String("brotha"))]),
         ])
       ),
     ),
  err => {
    Js.log(err);
    err |. Js.String.make;
  },
)
|. Future.mapOk(x => x |. Js.Nullable.toOption)
|. Future.get(instance =>
     switch (instance |. Belt.Result.getExn) {
     | exception e => Js.log(e)
     | instance =>
       Js.log("-----------------------");
       KeyModel.Instance.(
         switch (instance) {
         | Some(instance) =>
           Js.log(instance |. _id);
           Js.log(instance |. key);
           Js.log(instance |. num);
         | None => Js.log("--> Model not found")
         }
       );
     }
   );

/* module KeyedModel = [%mongoose {key: String, num: Int}]; */