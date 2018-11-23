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

typedef long double scalar;

#define writeAttributeScalar(aName, aVal) writeAttributeLongDouble(aName, aVal)

#define GEOMETRY_LINE 	(220)
#define GEOMETRY_ARC 	(221)
#define GEOMETRY_SPIRAL	(222)

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
	int writeAttribute(const char*  aName, string  aVal) {
		return writeAttribute(aName, aVal.c_str());
	}
	int writeAttributeInt(const char* aName, int aVal) {
    		/* Add an attribute with name aName and value v to current element. */
    		int rc;
    		char vStr[180];
    		sprintf(vStr, "%d", aVal);
    		rc = writeAttribute( aName, vStr);
    		return rc;
	}
	int writeAttributeDouble(const char* aName, double aVal) {
    		/* Add an attribute with name aName and value v to current element. */
    		int rc;
    		char vStr[180];
    		sprintf(vStr, "%lf", aVal);
    		rc = writeAttribute( aName, vStr);
    		return rc;
	}
	int writeAttributeLongDouble(const char* aName, long double aVal) {
    		/* Add an attribute with name aName and value v to current element. */
    		int rc;
    		char vStr[180];
    		sprintf(vStr, "%.24Lg", aVal);
    		rc = writeAttribute( aName, vStr);
    		return rc;
	}
	
	int writeAttributeEnum(const char* aName, int idx, const char **strTable) {
    		/* Add an attribute with name aName and value v to current element. */
    		int rc;
    		rc = writeAttribute( aName, strTable[idx]);
    		return rc;
	}
	int writeAttributesXY(scalar x, scalar y) {
    		/* Add an attribute with name aName and value v to current element. */
		int rc;
    		rc = writeAttributeScalar( "x", x);
    		rc = writeAttributeScalar( "y", y);
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
class odrCurve {
    public:
	scalar s;
	scalar a;
	scalar b;
	scalar c;
	scalar d;
	odrCurve() {
		s = 0.;
		a = 0.;	b = 0.;
		c = 0.;	d = 0.;
	}
	int saveXodr(xmlWriter xmlFile,const char *eName) {
		xmlFile.writeElement(eName);
		xmlFile.writeAttributeScalar("sOffset", s);
		xmlFile.writeAttributeScalar("a", a);
		xmlFile.writeAttributeScalar("b", b);
		xmlFile.writeAttributeScalar("c", c);
		xmlFile.writeAttributeScalar("d", d);
		xmlFile.closeElement();
		return 1;
	}	
};

class odrHeight {
    public:
	scalar s;
	scalar inner;
	scalar outer;
	odrHeight() {
		s = 0.;
		inner = 0.;
		outer =0.;
	}
	odrHeight(double h) {
		inner = h;
		outer = h;
	}
	odrHeight(double h1, double h2) {
		inner = h1;
		outer = h2;
	}
	void set(double h) {
		inner = h;
		outer = h;
	}
	int saveXodr(xmlWriter xmlFile) {
		xmlFile.writeElement("height");
		xmlFile.writeAttributeScalar("sOffset", s);
		xmlFile.writeAttributeScalar("heightInner", inner);
		xmlFile.writeAttributeScalar("heightOuter", outer);
		xmlFile.closeElement();
		return 1;
	}
};

class odrMark {
    public:
	scalar s;
	short int type;
	short int weight;
	short int color;
	double width;
	const char *strTableType[3] = { "none", "solid", "broken"};
	const char *strTableWeight[2] = { "standard", "extra"};
	const char *strTableColor[3] = { "standard", "white", "yellow"};

	odrMark() {
		s = 0.;
		type = 0;
		weight = 0;
		color = 0;
		width = 0.;
	}
	odrMark(double w) {
		width = w;
	}
	int saveXodr(xmlWriter xmlFile) {
		xmlFile.writeElement("roadMark");
		xmlFile.writeAttributeScalar("sOffset", s);
		xmlFile.writeAttributeEnum("type", type, strTableType);
		xmlFile.writeAttributeEnum("weight", weight, strTableWeight);
		xmlFile.writeAttributeEnum("color", color, strTableColor);
		xmlFile.writeAttributeScalar("width", width);
		xmlFile.closeElement();
		return 1;
	}	
};
class odrLink {
    public:
	int from;
	int to;
	odrLink() {
		from = 0;
		to = 0;
	}
	int saveXodr(xmlWriter xmlFile) {
		xmlFile.writeElement("link");
		  xmlFile.writeElement("predecessor");
		    xmlFile.writeAttributeInt("id", from);
		  xmlFile.closeElement();
		  xmlFile.writeElement("sucessor");
		    xmlFile.writeAttributeInt("id", to);
		  xmlFile.closeElement();
		xmlFile.closeElement();
		return 1;
	}
};

class odrLane {
    public:
	int id;
	unsigned short type;
	short level;
	odrLink link;
	odrCurve width;
	odrMark mark;
	odrHeight height;	
	const char *strTableType[4] = { "none", "driving", "border", "sidewalk" };
	odrLane(int nId, int nType) {
		id = nId;
		type = nType;
		level = 0;
		mark.width = 0.12;
	}
	int saveXodr(xmlWriter xmlFile) {
		xmlFile.writeElement("lane");
		xmlFile.writeAttributeInt("id", id);
		xmlFile.writeAttributeEnum("type", type, strTableType);
		xmlFile.writeAttributeInt("level", level);
		link.saveXodr(xmlFile);
		width.saveXodr(xmlFile,"width");
		mark.saveXodr(xmlFile);
		height.saveXodr(xmlFile);
		xmlFile.closeElement();
		return 1;
	}
};

class odrLaneSection {
    public:
	scalar s;
	vector<odrLane> left;
	vector<odrLane> center;
	vector<odrLane> right;
	odrLaneSection() {
		s = 0.;
		left.clear();
		center.clear();
		right.clear();
	}
	void setDefault() {
		s = 0.;
		odrLane centerLane(0,1);
		centerLane.mark.type = 2;
		center.push_back(centerLane);

		odrLane rightLane(-1,1);
		rightLane.width.a = 3.75;
		rightLane.mark.type = 1;
		right.push_back(rightLane);

		odrLane leftLane(1,1);
		leftLane.width.a = 3.75;
		leftLane.mark.type = 1;
		left.push_back(leftLane);

		odrLane rightBorder(-2,2);
		rightBorder.width.a = 0.7;
		right.push_back(rightBorder);

		odrLane leftBorder(2,2);
		leftBorder.width.a = 0.7;
		left.push_back(leftBorder);

		odrLane rightWalk(-3,3);
		rightWalk.width.a = 1.2;
		rightWalk.height.set(0.15);
		right.push_back(rightWalk);

		odrLane leftWalk(3,3);
		leftWalk.width.a = 1.2;
		leftWalk.height.set(0.15);
		left.push_back(leftWalk);

		odrLane rightMargin(-4,0);
		rightMargin.width.a = 4.7;
		right.push_back(rightMargin);

		odrLane leftMargin(4,0);
		leftMargin.width.a = 4.7;
		left.push_back(leftMargin);
	}
	int saveXodr(xmlWriter xmlFile) {
		unsigned i;
		xmlFile.writeElement("laneSection");
		xmlFile.writeAttributeScalar("s", s);
		
		  xmlFile.writeElement("left");
		  for( i=0 ; i<left.size() ; i++ )
		    left[i].saveXodr(xmlFile);
		  xmlFile.closeElement();
		
		  xmlFile.writeElement("center");
		  for( i=0 ; i<center.size() ; i++ )
		    center[i].saveXodr(xmlFile);
		  xmlFile.closeElement();
		printf("vai gravar right\n");
		  xmlFile.writeElement("right");
		  for( i=0 ; i<right.size() ; i++ )
		    right[i].saveXodr(xmlFile);
		  xmlFile.closeElement();

		xmlFile.closeElement();
		return 1;
	}
	void print() {
		printf("LaneSection, s: %Lf\n", s);
		
		printf("  Left, %lu lanes\n", left.size() );
		printf("  Center, %lu lanes\n", center.size() );
		printf("  Right, %lu lanes\n", right.size() );
	}
};



class odrLanes {
    public:
	odrLaneSection laneSection;
	void setDefault() {
		laneSection.setDefault();
	}
	int saveXodr(xmlWriter xmlFile) {
		xmlFile.writeElement("lanes");
		  laneSection.saveXodr(xmlFile);
		xmlFile.closeElement();
		return 1;
	}
	void print() {
		laneSection.print();
	}
};


class PlanView : public vector<odrGeometry> {
    public:
	scalar length;
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
	odrLanes lanes;
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
		int res = planView.loadPts(fname);
		if ( res ) {
			lanes.setDefault();
			length = planView.length;
		}
		return res;
	}
	int savePts(const char* fname) {
		return planView.savePts(fname);
	}
	void print();
};



