# Tiny server
Light weight static files server 
that can convert json into xml to show in webbrowsers
Just put in address line http://localhost:8181/foo/bar.json or 
http://localhost:8181/image/test.png:

## Build and Run
First clone to your directory

```Shell
git clone https://github.com/killuglyjoe/Json2XmlServerConverter.git
cd Json2XmlServerConverter
```

Now you can build it in two ways:
First:

```Shell
make
```
then run
```Shell
./nweb
```
Second:
open in QtCreator qnweb.pro and build it

or run in command line

```Shell
qmake && make
```

then run
```Shell
./QNweb
```

