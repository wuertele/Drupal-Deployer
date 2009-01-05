/* $Id$ */

DESCRIPTION
-----------

The central API call, unitsapi_convert($value, $from, $to, $details = FALSE), uses the International System of Units (SI) 
conversion factors to convert measurement units.  For more, see: http://physics.nist.gov/Pubs/SP811/contents.html

This module is only an API and does not have an user interface.

EXAMPLES
------

// Convert kilometer to feet
$result = unitsapi_convert(1.5, 'kilometer', 'foot');
// $result == 4921.259843

// Convert Fahrenheit to Kelvin
$result = unitsapi_convert(55, 'fahrenheit', 'kelvin');
// $result == 285.927778 

// Convert US liquid ounces to Imperial pints with a detailed array of the conversion
$result = unitsapi_convert(50, 'us ounce', 'imperial pint', TRUE);
// $result == Array ([result] => 2.602107, [from] => US ounces, [to] => Imperial pints)

SUPPORTED UNITS
------
All units are stored in units.xml.

Length:
millimeter
centimeter
decimeter
meter
kilometer
foot 
inch  
mile  
yard 

Volume:
cubic foot 
cubic inch    
cubic mile 
cubic yard  
cup (U.S.) 
gallon (Imperial and U.S.)
liter   
ounce (Imperial and U.S.)
pint (Imperial and U.S.)
quart (Imperial and U.S.) 
tablespoon 
teaspoon 

Time:
day  
hour 
minute    
year 

Temperature:
Celsius  
Fahrenheit (°F)   
kelvin (K) 
 
FUTURE PLANS
------
1) Add additional measurement units (weights, etc)
2) Add additional automated tests

ISSUES
------
Any issues are welcome to the Units API issue queue: http://drupal.org/project/issues/unitsapi).
Patches should be tested with the provided automated tests. 

If testing conversions, use the assertion:
$this->assertUnitConversion($value, $from, $to, $expected, $group)

SPONSORED BY
------
This module is sponsored by Raspberry Man LLC (http://www.raspberryman.com)
