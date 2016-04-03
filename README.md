# Arduino Post Values

Program that sends sensors values from an Arduino board to an RESTful API. You can see the usage on [this post](http://dreamingecho.es/physical-software-made-easy-with-arduino-and-ruby-on-rails/).

## About

This is an example of how to send values from sensors connected to an Arduino board to a web application through a RESTful API. For this example, I used a very simple web application that I made for sampling purpose (you could find it [here](https://github.com/dreamingechoes/sensors_admin_panel)).

It uses an **ENC28J60 Ethernet** chip and a **DHT11 Temperature and humidity** sensor.

## Original author

Iván González, *a.k.a* [dreamingechoes](https://github.com/dreamingechoes)

## Schema

This is a schema of the setup made with Fritzing:

![Schema](/documents/schema.png)

## Dependencies

You will need to download and add to your Arduino IDE this two libraries:

* [Ethercard](https://github.com/jcw/ethercard)
* [Dht11_Library](https://github.com/adalton/arduino/tree/master/projects/Dht11_Library)

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request

## License

**Arduino Post Values** is released under the [MIT License](http://www.opensource.org/licenses/MIT).
