open Jest;
open Expect;
type queryObj = {
  query: option(string),
  array: option(array(string)),
};

describe("query string utils", () => {
  test("stringify works with single and multiple values", () => {
    let qs =
      QueryString.Stringify.(
        make([|
          ("query", string("text")),
          ("filter", string("name")),
          ("array", array([|"a", "b", "c"|])),
        |])
      );

    // tested against https://www.urlencoder.io/
    expect(qs) |> toEqual("query=text&filter=name&array=a&array=b&array=c");
  });

  test("stringify encodes special characters", () => {
    let qs =
      QueryString.Stringify.(
        make([|
          ("query", string("search *1! -/0'")),
          ("array", array([|";_=.,+", "b~()[[]"|])),
        |])
      );

    expect(qs)
    |> toEqual(
         "query=search%20%2A1%21%20-%2F0%27&array=%3B_%3D.%2C%2B&array=b~%28%29%5B%5B%5D",
       );
  });

  test("parse correctly decodes single and multiple values", () => {
    let qs = "query=text&filter=name&array=a&array=b&array=c";
    let queryObj =
      QueryString.Parse.(
        {
          let map = parse(qs);
          {query: map |> string("query"), array: map |> array("array")};
        }
      );

    expect(queryObj)
    |> toEqual({query: Some("text"), array: Some([|"a", "b", "c"|])});
  });

  test("parse correctly decodes special characters", () => {
    let qs = "query=search%20%2A1%21%20-%2F0%27&array=%3B_%3D.%2C%2B&array=b~%28%29%5B%5B%5D";
    let queryObj =
      QueryString.Parse.(
        {
          let map = parse(qs);
          {query: map |> string("query"), array: map |> array("array")};
        }
      );

    expect(queryObj)
    |> toEqual({
         query: Some("search *1! -/0'"),
         array: Some([|";_=.,+", "b~()[[]"|]),
       });
  });
});
