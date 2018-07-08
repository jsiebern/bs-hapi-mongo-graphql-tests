open Migrate_parsetree;
open Ast_406;
open Ast_helper;
open Asttypes;
open Parsetree;

module Parse = {
  let recordEntriesToMongooseFields =
      (entries: list((Asttypes.loc(Longident.t), Parsetree.expression))) =>
    entries
    |> List.map(entry => {
         let (name, typ) = entry;
         (
           switch (name.txt) {
           | Longident.Lident(s) => s
           | _ =>
             Utils.raiseError(name.loc, Some("Please provide a valid name"))
           },
           switch (typ) {
           | {pexp_desc: Pexp_construct({txt: Longident.Lident(s)}, None)} =>
             switch (s) {
             | "String"
             | "Int"
             | "Float"
             | "Date"
             | "Boolean" => s
             | _ =>
               Utils.raiseError(
                 typ.pexp_loc,
                 Some(
                   "Mongo Field-Type can only be one of: String, Int, Float, Date, Boolean",
                 ),
               )
             }
           | _ =>
             Utils.raiseError(
               typ.pexp_loc,
               Some(
                 "Mongo Field-Type can only be one of: String, Int, Float, Date, Boolean",
               ),
             )
           },
         );
       });
};

module Generate = {
  let jsLog = (exp: Parsetree.expression) =>
    Ast_helper.Vb.mk(
      Ast_helper.Pat.any(),
      Ast_helper.Exp.apply(
        Ast_helper.Exp.ident(
          Location.mknoloc(
            Ast_406.Longident.Ldot(Longident.parse("Js"), "log"),
          ),
        ),
        [(Nolabel, exp)],
      ),
    );
};
