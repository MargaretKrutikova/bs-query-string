open Jest;
open Expect;

type paramsObj = {
  query: string,
  array: array(string),
};

type paramsObjWithOption = {
  optionQuery: option(string),
  array: array(string),
};

describe("parse", () => {
  test("parses string and array values correctly", () => {
    let qs = "query=text&&array=a&array=b&array=c";

    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        query: queryObj |> item("query", string),
        array: queryObj |> item("array", array),
      };

    expect(parsedObj) |> toEqual({query: "text", array: [|"a", "b", "c"|]});
  });

  test("decodes special characters correctly", () => {
    let qs = "query=search%20%2A1%21%20-%2F0%27&array=%3B_%3D.%2C%2B&array=b~%28%29%5B%5B%5D";
    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        query: queryObj |> item("query", string),
        array: queryObj |> item("array", array),
      };

    expect(parsedObj)
    |> toEqual({query: "search *1! -/0'", array: [|";_=.,+", "b~()[[]"|]});
  });

  test("throws if value is not present in query string", () => {
    let qs = "array=1&array=2";
    let queryObj = Parse.toQueryObj(qs);
    let parse = () =>
      Parse.{
        query: queryObj |> item("query", string),
        array: queryObj |> item("array", array),
      };

    expect(parse) |> toThrow;
  });

  test("throws if array was parsed for single value", () => {
    let qs = "q=1&q=2&array=1&array=2";
    let queryObj = Parse.toQueryObj(qs);
    let parse = () =>
      Parse.{
        query: queryObj |> item("query", string),
        array: queryObj |> item("array", array),
      };

    expect(parse) |> toThrow;
  });

  test("parses optional values correctly", () => {
    let qs = "array=1&array=2";
    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        optionQuery: queryObj |> optional(item("query", string)),
        array: queryObj |> item("array", array),
      };

    expect(parsedObj) |> toEqual({optionQuery: None, array: [|"1", "2"|]});
  });

  test("parses default values correctly", () => {
    let qs = "";
    let queryObj = Parse.toQueryObj(qs);
    let parsedObj =
      Parse.{
        optionQuery: queryObj |> optional(item("query", string)),
        array: queryObj |> withDefault([|"1", "2"|], item("array", array)),
      };

    expect(parsedObj) |> toEqual({optionQuery: None, array: [|"1", "2"|]});
  });
});
