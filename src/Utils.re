let addToPath = (~path, ~qs) => {
  switch (qs) {
  | "" => path
  | q => {j|$path?$q|j}
  };
};
