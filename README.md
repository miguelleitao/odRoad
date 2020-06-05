# odRoad
OpenDrive Road C++ Class

Simple C++ Class to handle a road definition following OpenDRIVE specification.

This is an exprerimental and not complete implementation.

Provided methods:
  
*	int saveXodr();
*	int loadPts();
*	int savePts();
*	int loadXodr();        // Not implemented

This repo also includes pts2xodr, a converter from PTS (road axis definition) to Xodr.

## Compile and Test
```
make
./pts2xodr sine.pts sine.xml
xmllint --format sine.xml >sine.xodr
```
## More info
Also check https://github.com/miguelleitao/xodr2osg.git
