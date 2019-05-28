[%raw {|require('!style-loader!css-loader!./index.css')|}];

module Demo = {
  let wrap = (msg, setState, value) => {
    setState(_ => Some(value));
    Js.log({j|$msg: $value|j});
  };

  [@react.component]
  let make = () => {
    let (country1, setCountry1) = React.useState(() => None);
    let (country2, setCountry2) = React.useState(() => Some("dz"));
    let (country3, setCountry3) = React.useState(() => Some("ag"));
    <React.Fragment>
      <div className="example">
        <div className="description">
        {React.string("No value selected")}
        </div>
        <CountrySelect
          country=country1
          onChange={wrap("Select 1", setCountry1)}
        />
      </div>
      <div className="example">
        <div className="description">
        {React.string("Initial value provided")}
        </div>
        <CountrySelect
          country=country2
          onChange={wrap("Select 2", setCountry2)}
        />
      </div>
      <div className="example">
        <div className="description">
        {React.string("Custom class")}
        </div>
        <CountrySelect
          className="custom"
          country=country3
          onChange={wrap("Select 3", setCountry3)}
        />
      </div>
    </React.Fragment>;
  };
};

ReactDOMRe.renderToElementWithId(<Demo />, "root");
