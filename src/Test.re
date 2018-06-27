let server =
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

Hapi.Server.Instance.Info.(Js.log(info));