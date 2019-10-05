# bs-query-string

Simple `reasonml` solution for dealing with query strings.

---

Inspired by [`bs-json`](https://github.com/glennsl/bs-json), `BsQueryString.Parse` works similarly to `Json.Decode` and `BsQueryString.Stringify` similarly to `Json.Encode`.

## Installation

```sh
npm i --save bs-query-string
```

Update your `bsconfig.json`:

```diff
"bs-dependencies": [
  ...
+ "bs-query-string",
]
```

## Parse query string

Example of parsing a query string into a record:

```reason
type productSearch = {
  query: string,
  sizes: array(string),
};

let parseQs = (qs: string): t => {
  let obj = BsQueryString.Parse.toQueryObj(qs);
  BsQueryString.Parse.{
    query: obj |> item("q", string),
    sizes: obj |> item("color", array),
  };
};
```

`item` will throw if the key is not found, or if multiple values were received for a single string value. You can use `optional` to return `None` or `withDefault` to assign default value:

```reason
type productSearch = {
  query: option(string),
  sizes: array(string),
};

let parseQs = (qs: string): t => {
  let obj = BsQueryString.Parse.toQueryObj(qs);
  BsQueryString.Parse.{
    query: obj |> optional(item("q", string)),
    sizes: obj |> withDefault([||], item("color", array)),
  };
};
```

This will produce following results:

```reason
// "q=jeans&size=S&size=XL"
let search = { q: Some("jeans"), sizes: [|"S", "XL"|] };

// "size=S&size=XL"
let search = { q: None, sizes: [|"S", "XL"|] };

// ""
let search = { q: None, sizes: [||] };
```

## Stringify

Example of stringifying a record into a query string:

```reason
type productSearch = {
  query: option(string),
  sizes: array(string),
};

let stringifySearch = (search: productSearch) => {
  Stringify.(
    toQs([|
      ("query", optional(search.query, string)),
      ("array", item(search.sizes, array)),
    |])
  );
};
```

`optional` will not include the key if the corresponding value is `None`.
