module SchemaDef =
  struct
    type t
    type def = {
      key: Mongoose.SchemaType.t;}[@@bs.deriving jsConverter]
    type defJs = < key :Mongoose.SchemaType.t  > Js.t
    type defValues = < key :string  > Js.t
    let def =
      { key = (Mongoose.SchemaType.make ~_type:String ~required:true ()) }
    let modelName = "KeyTest"
  end
