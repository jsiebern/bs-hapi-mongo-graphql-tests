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

/* Mongoose.Connection.connect("mongodb://127.0.0.1:27017");
   Mongoose.Connection.on(`connecting, () => Js.log("connecting to MongoDB..."));
   Mongoose.Connection.on(`connected, () => Js.log("MongoDB connected!")); */

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
  };

  module Schema = Mongoose.MakeSchema(SchemaDef);
  module Model = Mongoose.MakeModel(SchemaDef);

  let schema = Schema.schema;
  let model = Model.setup(schema);
};

/* let model = Model.setup(Schema.schema);
   Js.log(model |. Model.create({"key": "brotha"})); */

/* module KeyedModel = [%mongoose {key: String, num: Int}]; */
