type queryValue =
  | Single(string)
  | Multiple(array(string));

/**
 * encodeURIComponent won't encode: - _ . ! ~ * ' ( )
 * according to https://tools.ietf.org/html/rfc3986#page-13
 * unreserved characters: - _ . ~
 * reserved: ! * ( ) '
 * thus JS doesn't cover ! * ( ) ', so we will help it.
 *  */
let encode = value =>
  Js.Global.encodeURIComponent(value)
  |> Js.String.replaceByRe([%re "/\!/"], "%21")
  |> Js.String.replaceByRe([%re "/\(/"], "%28")
  |> Js.String.replaceByRe([%re "/\)/"], "%29")
  |> Js.String.replaceByRe([%re "/\*/"], "%2A")
  |> Js.String.replaceByRe([%re "/\'/"], "%27");

module Stringify = {
  let string = val_ => Single(val_);
  let array = arr => Multiple(arr);

  let encodePair = (key, value) => encode(key) ++ "=" ++ encode(value);

  let itemToQs = (key, item) =>
    switch (item) {
    | Single(value) => encodePair(key, value)
    | Multiple(values) =>
      values->Belt.Array.map(value => encodePair(key, value))
      |> Js.Array.joinWith("&")
    };

  let make = (params: array((string, queryValue))) =>
    params->Belt.Array.map(((key, item)) => itemToQs(key, item))
    |> Js.Array.joinWith("&");

  let addToPath = (~path, ~qs) => {
    switch (qs) {
    | "" => path
    | q => {j|$path?$q|j}
    };
  };
};

module Parse = {
  exception ParseError(string);

  type queryObj = Belt_MapString.t(queryValue);

  let addToMap = (map, keyValue) => {
    switch (Js.String.split("=", keyValue)) {
    | [|"", ""|] => map
    | [|key, encodedValue|] =>
      let value = Js.Global.decodeURIComponent(encodedValue);
      switch (map->Belt_MapString.get(key)) {
      | None => map->Belt_MapString.set(key, Single(value))
      | Some(Single(singleVal)) =>
        map->Belt_MapString.set(key, Multiple([|singleVal, value|]))
      | Some(Multiple(multiVal)) =>
        let arrayValue = Js.Array.concat([|value|], multiVal);
        map->Belt_MapString.set(key, Multiple(arrayValue));
      };
    | _ => map
    };
  };

  let parse = (query): queryObj => {
    let queryMap = Belt_MapString.empty;

    let pairs = Js.String.split("&", query);
    Array.fold_left(addToMap, queryMap, pairs);
  };

  let string = (key, queryObj) => {
    queryObj
    ->Belt_MapString.get(key)
    ->Belt.Option.map(item =>
        switch (item) {
        | Single(val_) => val_
        | Multiple(array) =>
          let strArray = array |> Js.Json.stringArray |> Js.Json.stringify;
          raise(ParseError("Expected single value, got " ++ strArray));
        }
      );
  };

  let array = (key, queryObj) => {
    queryObj
    ->Belt_MapString.get(key)
    ->Belt.Option.map(item =>
        switch (item) {
        | Single(val_) => [|val_|]
        | Multiple(array) => array
        }
      );
  };
};
