open QueryTypes;

type itemValue =
  | Item(queryValue)
  | Omit;

type queryItem = (string, itemValue);
type queryArray = array(queryItem);

type encoder('a) = 'a => queryValue;

/**
 * encodeURIComponent won't encode: - _ . ! ~ * ' ( )
 * according to https://tools.ietf.org/html/rfc3986#page-13
 * unreserved characters: - _ . ~
 * reserved: ! * ( ) '
 * JS doesn't cover ! * ( ) ', so we will help it.
 */
let encode = value =>
  Js.Global.encodeURIComponent(value)
  |> Js.String.replaceByRe([%re "/\\!/"], "%21")
  |> Js.String.replaceByRe([%re "/\\(/"], "%28")
  |> Js.String.replaceByRe([%re "/\\)/"], "%29")
  |> Js.String.replaceByRe([%re "/\\*/"], "%2A")
  |> Js.String.replaceByRe([%re "/\'/"], "%27");

let encodePair = (key, value) => encode(key) ++ "=" ++ encode(value);

let itemToQueryString = ((key, item)) =>
  switch (item) {
  | Omit => ""
  | Item(i) =>
    switch (i) {
    | Single(value) => encodePair(key, value)
    | Multiple(values) =>
      values->Belt.Array.map(value => encodePair(key, value))
      |> Js.Array.joinWith("&")
    }
  };

let string = val_ => Single(val_);
let array = arr => Multiple(arr);

let item = (val_, encoder) => Item(encoder(val_));

let omit = (val_, encoder, shouldOmit) =>
  shouldOmit(val_) ? Omit : item(val_, encoder);

let toQs = (items: array(queryItem)) =>
  items
  ->Belt.Array.map(itemToQueryString)
  ->Belt.Array.keep(pair => pair != "")
  |> Js.Array.joinWith("&");
