let mapper = argv => {
  open Ast_402;
  open Asttypes;
  open Parsetree;
  open Ast_mapper;
  open Ast_helper;
  open Parsetree;
  open Location;
  open Asttypes;
  let schema =
    switch (find_schema_file(Sys.getcwd())) {
    | Some(filename) => Read_schema.read_schema_file(filename)
    | None => raise(Schema_file_not_found)
    };
  let is_ast_output =
    switch (List.find((==)("-ast-out"), argv)) {
    | _ => true
    | exception Not_found => false
    };
  let module_expr = (mapper, mexpr) =>
    switch (mexpr) {
    | {
        pmod_desc:
          [@implicit_arity] Pmod_extension({txt: "graphql", loc}, pstr),
      } =>
      switch (pstr) {
      | PStr([
          {
            pstr_desc:
              [@implicit_arity]
              Pstr_eval(
                {
                  pexp_loc: loc,
                  pexp_desc:
                    Pexp_constant(
                      [@implicit_arity] Const_string(query, delim),
                    ),
                },
                _,
              ),
          },
        ]) =>
        let lexer = Graphql_lexer.make(query);
        let delimLength =
          switch (delim) {
          | Some(s) => 2 + String.length(s)
          | None => 1
          };
        switch (Graphql_lexer.consume(lexer)) {
        | Result.Error(e) =>
          raise(
            Location.Error(
              Location.error(
                ~loc=add_loc(delimLength, loc, e.span),
                fmt_lex_err(e.item),
              ),
            ),
          )
        | Result.Ok(tokens) =>
          let parser = Graphql_parser.make(tokens);
          switch (Graphql_parser_document.parse_document(parser)) {
          | Result.Error(e) =>
            raise(
              Location.Error(
                Location.error(
                  ~loc=add_loc(delimLength, loc, e.span),
                  fmt_parse_err(e.item),
                ),
              ),
            )
          | Result.Ok(document) =>
            let config = {
              Generator_utils.map_loc: add_loc(delimLength, loc),
              delimiter: delim,
              output_mode:
                if (is_ast_output) {Generator_utils.Apollo_AST} else {
                  Generator_utils.String
                },
              full_document: document,
              schema,
            };
            switch (Validations.run_validators(config, document)) {
            | Some(errs) =>
              Mod.mk(
                Pmod_structure(
                  List.map(
                    ((loc, msg)) => [%stri [%e make_error_expr(loc, msg)]],
                    errs,
                  ),
                ),
              )
            | None =>
              let parts =
                Result_decoder.unify_document_schema(config, document);
              Output_bucklescript_module.generate_modules(config, parts);
            };
          };
        };
      | _ =>
        raise(
          Location.Error(
            Location.error(
              ~loc,
              "[%graphql] accepts a string, e.g. [%graphql {| { query |}]",
            ),
          ),
        )
      }
    | other => default_mapper.module_expr(mapper, other)
    };
  To_current.copy_mapper({...default_mapper, module_expr});
};
