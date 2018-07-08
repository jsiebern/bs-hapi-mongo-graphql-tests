open Migrate_parsetree;
open Ast_406;
open Ast_mapper;
open Ast_helper;
open Asttypes;
open Parsetree;
open Longident;

/* let rec module_expr = (mapper, e) =>
   switch (e) {
   | {
       pmod_desc:
         [@implicit_arity] Pmod_extension({txt: "mongoose", loc}, pstr),
     } =>
     switch (pstr) {
     | PStr([
         {
           pstr_desc:
             Pstr_eval(
               {pexp_loc: loc, pexp_desc: Pexp_record(entries, rLoc)},
               _,
             ),
         },
       ]) =>
       let keys = entries |> Helpers.Parse.recordEntriesToMongooseFields;

       Mod.mk(
         Pmod_structure(
           keys
           |> List.map(keyVal =>
                Ast_helper.Str.value(
                  Nonrecursive,
                  [
                    Helpers.Generate.jsLog(
                      Ast_helper.Exp.constant(
                        Pconst_string(keyVal |> fst, None),
                      ),
                    ),
                    Helpers.Generate.jsLog(
                      Ast_helper.Exp.constant(
                        Pconst_string(keyVal |> snd, None),
                      ),
                    ),
                  ],
                )
              ),
         ),
       );
     | _ => Utils.raiseError(loc, None)
     }
   | _ => default_mapper.module_expr(mapper, e)
   }; */

let rec module_binding = (mapper, e) =>
  switch (e) {
  | {
      pmb_name: {txt: moduleName, loc: moduleLoc},
      pmb_expr: {
        pmod_desc:
          [@implicit_arity] Pmod_extension({txt: "mongoose", loc}, pstr),
      },
    } =>
    switch (pstr) {
    | PStr([
        {
          pstr_desc:
            Pstr_eval(
              {pexp_loc: loc, pexp_desc: Pexp_record(entries, rLoc)},
              _,
            ),
        },
      ]) =>
      let keys = entries |> Helpers.Parse.recordEntriesToMongooseFields;

      Ast_helper.Mb.mk(
        Location.mkloc(moduleName, moduleLoc),
        Mod.mk(
          Pmod_structure([
            Ast_helper.Str.value(
              Nonrecursive,
              [
                Helpers.Generate.jsLog(
                  Ast_helper.Exp.constant(Pconst_string(moduleName, None)),
                ),
              ],
            ),
            ...keys
               |> List.map(keyVal =>
                    Ast_helper.Str.value(
                      Nonrecursive,
                      [
                        Helpers.Generate.jsLog(
                          Ast_helper.Exp.constant(
                            Pconst_string(
                              (keyVal |> fst) ++ " => " ++ (keyVal |> snd),
                              None,
                            ),
                          ),
                        ),
                      ],
                    )
                  ),
          ]),
        ),
      );
    | _ => Utils.raiseError(loc, None)
    }
  | _ => default_mapper.module_binding(mapper, e)
  };

let mapper = (_, _) => {...default_mapper, module_binding};

Driver.register(
  ~name="mongoose",
  Migrate_parsetree.Versions.ocaml_406,
  mapper,
);
