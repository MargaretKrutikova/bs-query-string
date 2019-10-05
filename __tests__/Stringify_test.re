open Jest;
open Expect;

type testQueryObj = {
  query: option(string),
  array: option(array(string)),
};

describe("stringify", () => {
  test("stringifies string and array values", () => {
    let qs =
      Stringify.(
        toQs([|
          ("query", item("text", string)),
          ("filter", item("name", string)),
          ("array", item([|"a", "b", "c"|], array)),
        |])
      );

    // tested against https://www.urlencoder.io/
    expect(qs) |> toEqual("query=text&filter=name&array=a&array=b&array=c");
  });

  test("encodes special characters correctly", () => {
    let qs =
      Stringify.(
        toQs([|
          ("query", item("search *1! -/0'", string)),
          ("array", item([|";_=.,+", "b~()[[]"|], array)),
        |])
      );

    // tested against https://www.urlencoder.io/
    expect(qs)
    |> toEqual(
         "query=search%20%2A1%21%20-%2F0%27&array=%3B_%3D.%2C%2B&array=b~%28%29%5B%5B%5D",
       );
  });

  test("stringify allows to omit item", () => {
    let value = "";
    let isEmpty = v => v == "";

    let qs =
      Stringify.(
        toQs([|
          ("query", item("text", string)),
          ("filter", item("name", string)),
          ("empty", omit(value, string, isEmpty)),
        |])
      );

    expect(qs) |> toEqual("query=text&filter=name");
  });

  test("stringify doesn't include empty arrays", () => {
    let qs =
      Stringify.(
        toQs([|
          ("query", item("text", string)),
          ("array", item([||], array)),
        |])
      );
    expect(qs) |> toEqual("query=text");
  });
});
