
let makeProps: (
    ~className: 'className=?,
    ~country: 'country,
    ~onChange: 'onChange,
    ~key: string=?,
    unit) =>
    {. "className": option('className),
        "country": 'country,
        "onChange": 'onChange}

let make: {.
  "className": option(string),
  "country": option(string),
  "onChange": string => unit} =>
  React.element;

