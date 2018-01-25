

# UCCP_Server

This is a server application of a writing books platform . mobile client reposetory link will be added here as soon as posible .
This app created with [Poco c++ libraries](https://pocoproject.org/) , [mongo database](https://www.mongodb.com/) and [c++ 2014 standards](https://en.wikipedia.org/wiki/C%2B%2B14) .I'm developing this application only on linux and I don't know if it works on windows too and I don't care too. 

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

* [cmake](https://cmake.org/install/)
* [Poco instalation](https://pocoproject.org/docs/00200-GettingStarted.html)

### Installing

After installing [Poco](https://pocoproject.org/) you only need to open project with [Qt creator](http://doc.qt.io/qtcreator/) or any other ide and run it . If you want to use application on a real server you will need to set database host and port in code too .

After running you can use [API Documentation](APIs.md) to develope your client and use it .

## Running the tests

If you want to test performance you will need to comment all std::clogs  , because program will crash if you don't  comment them.This std::clog are temprary for now i will develope a good log system instade of using std::clog soon .

## Deployment

I'm developing this application to be a grate writing platform and I try to document it good to be a very good example of using Poco for developing a webservice and also a website in future .


> Modules
- [x] Database
- [ ] Server
- [ ] Controllers
- [ ] TemplateEngine
- [ ] FireWall
- [ ] Advertising
- [ ] Payment
- [ ] Statistics

## Built With

* [cmake](https://cmake.org/documentation/) - Software development tools
* [Poco](https://pocoproject.org/docs/) - The Server libraries used

## Commenting

I use [doxygen documentation style](http://doxygen.nl/manual.html) for documenting and I specify priority of TODOs with P[0-3] . e.g :

```
// TODO P[0] Fix Most important bug that crash application when user send an empty json
// TODO P[1] add important API
// TODO P[2] add an exciting feature 
```

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/FONQRI/UCCP_Server/tags). 

## Authors

* **Behnam Sabaghi** - *Initial work* - [FONQRI](https://github.com/FONQRI)


## License

This project is licensed under the GPL V3  License - see the [Licence](LICENSE) file for details

## Acknowledgments

* [Modern Cpp](http://moderncpp.ir/) website 


