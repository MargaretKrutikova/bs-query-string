type queryValue =
  | Single(string)
  | Multiple(array(string));

/** stringify */
// following are not encoded: ~!*()'
/*
 TODO: check those
 .replace('!', '%21')
 .replace('\'', '%27')
 .replace('(', '%28')
 .replace(')', '%29')
 .replace('*', '%2A')
 */
let encodePair = (key, value) =>
  Js.Global.encodeURIComponent(key)
  ++ "="
  ++ Js.Global.encodeURIComponent(value);

let itemToQs = (key, item) =>
  switch (item) {
  | Single(value) => encodePair(key, value)
  | Multiple(values) =>
    values->Belt.Array.map(value => encodePair(key, value))
    |> Js.Array.joinWith("&")
  };

let stringify = (params: array((string, queryValue))) =>
  params->Belt.Array.map(((key, item)) => itemToQs(key, item))
  |> Js.Array.joinWith("&");

let addToPath = (~path, ~qs) => {
  switch (qs) {
  | "" => path
  | q => {j|$path?$q|j}
  };
};

/** parse */
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
      let arrayValue = Js.Array.concat(multiVal, [|value|]);
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
