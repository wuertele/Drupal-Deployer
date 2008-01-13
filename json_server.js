// $Id$
Drupal.service = function(method, parameters, success) {
  if (!this.servicesClient) {
    return new Drupal.service(method, parameters, success);
  }
  parameters.method = method;
  parsed = this.parse(parameters);
  this.ajax(parsed, success);
}
Drupal.service.prototype = {
  servicesClient: true,
  parse: function(parameters) {
    return $.param(this._parse([], [], parameters));
  },
  _parse: function(currentData, currentNesting, parameters) {
    for (index in parameters) {
      data = parameters[index];
      currentNesting.push(encodeURIComponent(index));
      if (typeof data == 'object') {
        currentData = this._parse(currentData, currentNesting, data);
      }
      else {
        currentData.push({name: this._makeURI(currentNesting), value: encodeURIComponent(data) });
      }
      currentNesting.pop();
    }
    return currentData;
  },
  _makeURI: function(data) {
    output = data.shift();
    for (i in data) {
      output += '['+ data[i] +']';
    }
    return output;
  },
  ajax: function(data, success) {
    $.ajax({
      url: Drupal.settings.basePath +"?q=services/json",
      type: "POST",
      data: data,
      success: function(data) {
        parsed = Drupal.parseJson(data);
        success(parsed['status'], parsed['data']);
      },
    });
  }
};
