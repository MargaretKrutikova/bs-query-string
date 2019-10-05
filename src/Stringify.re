open QueryTypes;

type queryItem = (string, queryValue);
type queryArray = array(queryItem);

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

let string = val_ => Single(val_);
let array = arr => Multiple(arr);

let add = (key, queryValue, queryArray) =>
  queryArray->Belt.Array.concat([|(key, queryValue)|]);

let addOptional = (key, queryValue, ~skip, queryArray) =>
  skip ? queryArray : queryArray |> add(key, queryValue);

let toQs = (items: array(queryItem)) =>
  items
  ->Belt.Array.map(itemToQueryString)
  ->Belt.Array.keep(pair => pair != "")
  |> Js.Array.joinWith("&");

let withPath = (~path, ~qs) => {
  switch (qs) {
  | "" => path
  | q => {j|$path?$q|j}
  };
};
