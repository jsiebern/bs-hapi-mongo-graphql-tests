let raiseError = (loc: Location.t, message: option(string)) =>
  raise(
    Location.Error(
      Location.error(
        ~loc,
        switch (message) {
        | Some(m) => m
        | None => "[%mongoose] accepts a record, e.g. [%mongoose { example: string }]"
        },
      ),
    ),
  );
