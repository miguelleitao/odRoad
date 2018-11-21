/*
 * odRoad
 *
 */

#include <string>
#include <vector>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>


#define MY_ENCODING "ISO-8859-1"
#define PTS_MAX_LINE_SIZE (80)

using namespace std;

typedef double scalar;

#define GEOMETRY_LINE 	(220)
#define GEOMETRY_ARC 	(221)
#define GEOMETRY_SPIRAL	(222)

class odrGeometry {
    public:
	odrGeometry() {
		type = 0;
		s = 0.;
		x = 0.;
		y = 0.;
		hdg = 0.;
		length = 0.;
	}
	unsigned char type;
	scalar s;
	scalar x;
	scalar y;
	scalar hdg;
	scalar length;
};

class odrLine : public odrGeometry {
    public:
	odrLine() {
		type = GEOMETRY_LINE;
	}
};
class odrArc : public odrGeometry {
    public:
	scalar curv;
	odrArc() {
		type = GEOMETRY_ARC;
		curv = 0.;
	}
};
class odrSpiral : public odrGeometry {
    public:
	scalar curvIn;
	scalar curvOut;
	odrSpiral() {
		type = GEOMETRY_SPIRAL;
		curvIn = 0.;
		curvOut = 0.;
	}
};

//typedef vector<Geometry> PlanView;

class PlanView : public vector<odrGeometry> {
    public:
	int loadPts(const char * fname);
	int savePts(const char * fname);
	int saveXodr(class xmlWriter);
	void print();
};

class odRoad {
    public:
	string name;
	scalar length;
	int id;
	int junction;

	PlanView planView;

	odRoad() {
		name = "";
		length = 0.;
		id = -1;
		junction = 0;
	}
	int loadXodr(string &fname);
	int saveXodr(const char* fname);
	int loadPts(const char* fname) {
		name = fname;
		return planView.loadPts(fname);
	}
	void print();
};


class xmlWriter {
    public:
	const char* fname;
	xmlDocPtr xmlDoc;
	xmlTextWriterPtr xmlFile;
	xmlWriter() {
		fname = "";
	}
	void open(const char *file_name) {
		fname = file_name;
		xmlFile = xmlNewTextWriterDoc(&xmlDoc, 0);

  		if ( xmlFile == NULL ) {
        		fprintf(stderr, "xmlWriter: Error creating the xml writer\n");
        		return;
		}
		/* Start the document with the xml default for the version,
    		 * encoding ISO 8859-1 and the default for the standalone
    		 * declaration. */
    		int rc = xmlTextWriterStartDocument(xmlFile, NULL, MY_ENCODING, NULL);
    		if (rc < 0) {
        		printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        		return;
    		}	

    	}
	xmlWriter(char *file_name) {
		open(file_name);
	}

	void close() {
		printf("gravou tudo\n");
    		xmlFreeTextWriter(xmlFile);

    		printf("libertou writer\n");
    		int nb = xmlSaveFileEnc(fname, xmlDoc, MY_ENCODING);
    		if ( nb>=0 )	fprintf(stderr, "File saved with %d Bytes\n", nb);
    		else		fprintf(stderr, "Error saving file\n");
    		xmlFreeDoc(xmlDoc);

    		printf("libertou tudo\n");
	}
	int writeElement(const char* eName) {
		int rc;
		/* Start an element named "eName" as child of current element. */
		rc = xmlTextWriterStartElement(xmlFile, BAD_CAST eName);
		if (rc < 0) 
			fprintf(stderr, "Error at xmlWriteElement, writing element '%s'\n", eName);
		return rc;
	}
	int writeAttribute(const char*  aName, const char*  aVal) {
    		/* Add an attribute with name aName and value aVal to current element. */
    		int rc;
    		rc = xmlTextWriterWriteAttribute(xmlFile, BAD_CAST aName, BAD_CAST aVal);
    		if (rc < 0) 
			fprintf(stderr,"error writing attribute '%s'\n", aName);
		return rc;
	}
	int writeAttributeInt(const char* aName, int aVal) {
    		/* Add an attribute with name aName and value v to current element. */
    		int rc;
    		char vStr[180];
    		sprintf(vStr,"%d",aVal);
    		rc = writeAttribute( aName, vStr);
    		return rc;
	}
	int writeAttributeDouble(const char* aName, double aVal) {
    		/* Add an attribute with name aName and value v to current element. */
    		int rc;
    		char vStr[180];
    		sprintf(vStr,"%lf",aVal);
    		rc = writeAttribute( aName, vStr);
    		return rc;
	}
	int writeAttributesXY(scalar x, scalar y) {
    		/* Add an attribute with name aName and value v to current element. */
		int rc;
    		rc = writeAttributeDouble( "x", x);
    		rc = writeAttributeDouble( "y", y);
    		return rc;
	}
	int closeElement() {
		/* Close the current element. */
		int rc;
		rc = xmlTextWriterEndElement(xmlFile);
		if (rc < 0) {
			printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
			return -1;
		}
		return rc;
	}

};

