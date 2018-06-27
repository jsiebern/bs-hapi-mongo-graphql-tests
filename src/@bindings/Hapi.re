module Utils = {};

module Server = {
  module Options = {
    module Compression = {
      [@bs.deriving abstract]
      type asBytes = {minBytes: int};

      type t =
        | Boolean(bool)
        | Bytes(int);
    };

    module Debug = {
      [@bs.deriving abstract]
      type definedDebug = {
        [@bs.optional]
        log: option(array(string)),
        [@bs.optional]
        request: option(array(string)),
      };

      type t =
        | Boolean(bool)
        | Defined(definedDebug);
    };

    module Load = {
      [@bs.deriving abstract]
      type t = {
        [@bs.optional]
        sampleInterval: int,
        [@bs.optional]
        maxHeapUsedBytes: int,
        [@bs.optional]
        maxRssBytes: int,
        [@bs.optional]
        maxEventLoopDelay: int,
      };

      let make = t;
    };

    module Router = {
      [@bs.deriving abstract]
      type t = {
        [@bs.optional]
        isCaseSensitive: bool,
        [@bs.optional]
        stripTrailingSlash: bool,
      };

      let make = t;
    };

    module State = {
      [@bs.deriving jsConverter]
      type isSameSite = [ | `Strict | `Lax];
      [@bs.deriving jsConverter]
      type encoding = [ | `none | `base64 | `base64json | `form | `iron];

      [@bs.deriving abstract]
      type t = {
        [@bs.optional]
        strictHeader: bool,
        [@bs.optional]
        ignoreErrors: bool,
        [@bs.optional]
        isSecure: bool,
        [@bs.optional]
        isHttpOnly: bool,
        [@bs.optional]
        isSameSite: option(isSameSite),
        [@bs.optional]
        encoding,
      };

      let make = t;
    };
  };
  module ServerOptions = {
    [@bs.deriving abstract]
    type t = {
      [@bs.optional]
      address: string,
      [@bs.optional]
      uri: string,
      [@bs.optional]
      autoListen: bool,
      [@bs.optional]
      compression: Options.Compression.t,
      [@bs.optional]
      debug_: Options.Debug.t,
      [@bs.optional]
      host: string,
      [@bs.optional]
      debug: Options.Load.t,
      [@bs.optional]
      port: int,
      [@bs.optional]
      router: Options.Router.t,
      [@bs.optional]
      state: Options.State.t,
      /* listener */
      /* cache */
      /* app */
      /* mime */
      /* plugins */
      /* routes */
      /* tls */
      /* port as number OR string */
    };

    let make = t;
  };

  module Instance = {
    module Info = {
      [@bs.deriving abstract]
      type t = {uri: string};
      let make = t;
    };
    type t;

    [@bs.send] external extStart : t => Js.Promise.t('a) = "start";
    let start = (server: t) => {
      let handleError = Js.String.make;
      server |. extStart |. FutureJs.fromPromise(handleError);
    };
    [@bs.get] external info : t => Info.t = "";
  };

  module Bindings = {
    [@bs.new] [@bs.module "hapi"]
    external createServer : ServerOptions.t => Instance.t = "Server";
  };

  let make = (options: ServerOptions.t) => Bindings.createServer(options);
};