exception ParseError(string);

type queryValue =
  | Single(string)
  | Multiple(array(string));

type queryObj = Belt.Map.String.t(queryValue);

/**
 * encodeURIComponent won't encode: - _ . ! ~ * ' ( )
 * according to https://tools.ietf.org/html/rfc3986#page-13
 * unreserved characters: - _ . ~
 * reserved: ! * ( ) '
 * JS doesn't cover ! * ( ) ', so we will help it.
 */
let encode = value =>
  Js.Global.encodeURIComponent(value)
  |> Js.String.replaceByRe([%re "/\!/"], "%21")
  |> Js.String.replaceByRe([%re "/\(/"], "%28")
  |> Js.String.replaceByRe([%re "/\)/"], "%29")
  |> Js.String.replaceByRe([%re "/\*/"], "%2A")
  |> Js.String.replaceByRe([%re "/\'/"], "%27");

let encodePair = (key, value) => encode(key) ++ "=" ++ encode(value);

let itemToQueryString = ((key, item)) =>
  switch (item) {
  | Single(value) => encodePair(key, value)
  | Multiple(values) =>
    values->Belt.Array.map(value => encodePair(key, value))
    |> Js.Array.joinWith("&")
  };

module Stringify = {
  let string = val_ => Single(val_);
  let array = arr => Multiple(arr);

  let make = (params: array((string, queryValue))) =>
    params->Belt.Array.map(itemToQueryString) |> Js.Array.joinWith("&");
};

let addToPath = (~path, ~qs) => {
  switch (qs) {
  | "" => path
  | q => {j|$path?$q|j}
  };
};

/** parse */

let addToMap = (map, keyValue) => {
  switch (Js.String.split("=", keyValue)) {
  | [|"", ""|] => map
  | [|key, encodedValue|] =>
    let valueToAdd = Js.Global.decodeURIComponent(encodedValue);
    let newValue =
      switch (map->Belt.Map.String.get(key)) {
      | None => Single(valueToAdd)
      | Some(Single(singleVal)) => Multiple([|singleVal, valueToAdd|])
      | Some(Multiple(multiVal)) =>
        Multiple(multiVal->Belt.Array.concat([|valueToAdd|]))
      };
    map->Belt.Map.String.set(key, newValue);
  | _ => map
  };
};

let parse = (query): queryObj => {
  let queryMap = Belt.Map.String.empty;

  let pairs = Js.String.split("&", query);
  Array.fold_left(addToMap, queryMap, pairs);
};

module Parse = {
  let string = (key, queryObj) => {
    queryObj
    ->Belt.Map.String.get(key)
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
    ->Belt.Map.String.get(key)
    ->Belt.Option.map(item =>
        switch (item) {
        | Single(val_) => [|val_|]
        | Multiple(array) => array
        }
      );
  };
};
