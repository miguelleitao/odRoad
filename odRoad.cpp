/*
 * odRoad
 *
 */

#include <math.h>
#include <cassert>
#include "odRoad.h"

int verbose = 0;

void CopyVecD(double *v, double *dest) {
        int i;
        for( i=0 ; i<3 ; i++ )
                dest[i] = v[i];
}

int GetLine(char **line, size_t *line_len, FILE *fin) {
        int nc = getline( line, line_len, fin);
        return nc;
}

int PlanView::loadPts(const char *fname) {
	FILE *fin;
    long vertexes = 0;
    long comments = 0;
    long invalid = 0;
    long lines = 0;
    double length;
    double total_length = 0.;
    int file_format = 6;
    double p[3], e[3];
    //double last_v[3];
    double last_p[3] = {0.,0.,0.};
    //double v[3];
	char *line = (char *) malloc (PTS_MAX_LINE_SIZE + 1);
	size_t line_len = PTS_MAX_LINE_SIZE;

	fin = fopen(fname,"r");
	if ( ! fin ) {
		fprintf(stderr, "Cannot open file '%s'\n", fname);
		return -1;
	}
	int done = false;
	while( ! done && ! feof(fin)) {
		int res = GetLine(&line, &line_len, fin);
		if ( feof(fin) ) break;
		if ( line[0]=='#' ) {
			comments++;
			continue;
		}
		if ( file_format==6 ) {
			res = sscanf(line,"%lf %lf %lf %lf %lf %lf\n", p+0, p+1, p+2, e+0, e+1, e+2);
			lines++;
			if ( verbose>=4 ) {
				printf("res %d, %lf %lf %lf\n",res,p[0],p[1],p[2]);
			}
			if ( res<3 ) {
				invalid++;
				if ( verbose>1 )
				  printf("Invalid line %ld: %s\n", lines, line);
				continue;
			}
			vertexes++;
			if ( res<6 ) e[0] = e[1] = e[2] = 0.;
		}
		if ( file_format==1 ) {
			if ( sscanf(line,"%lf", p+1) <1 ) {
				invalid++;
				continue;
			}
			p[0] = (double)vertexes;
			vertexes++;
		}
		length = sqrt( (p[0]-last_p[0])*(p[0]-last_p[0]) + (p[1]-last_p[1])*(p[1]-last_p[1]) );

		odrGeometry *geoNode = new odrLine();
		geoNode->s = total_length;
		geoNode->x = p[0];
		geoNode->y = p[1];
		geoNode->hdg = e[0];
		geoNode->length = length;
		push_back(*geoNode);
		
		total_length += length;

		CopyVecD(p,last_p);
	}
	printf("leu %ld segs\n", vertexes);
	return vertexes;
}

int PlanView::saveXodr(xmlWriter xmlFile) {
	unsigned i;	
	int rc=0;
	
	double total_length = 0.;
	xmlFile.writeElement("planView");
	for( i=0 ; rc>=0 && i<size() ; i++ ) {
		/* Start an element named "geometry" as child of planView. */
		rc = xmlFile.writeElement("geometry");

		/* Add an attribute with name "s" and value "1.0" to ORDER. */
		rc = xmlFile.writeAttributeDouble("s", total_length);

		odrGeometry geoNode = at(i);
		
		xmlFile.writeAttributesXY( geoNode.x, geoNode.y);
		xmlFile.writeAttributeDouble("hdg", geoNode.hdg);
		xmlFile.writeAttributeDouble("length", geoNode.length);

		/* Start an element named "line" as child of geometry. */
		rc = xmlFile.writeElement("line");
	
		/* Close the element named line. */
		rc = xmlFile.closeElement();

		assert( abs(total_length-geoNode.s)<0.001 );

		total_length += geoNode.length;
		/* Close the element named geometry. */
		rc = xmlFile.closeElement();
	}
	printf("gravou %u segs\n",i);
	return i;
}


void PlanView::print() {
	unsigned i;
	
	double total_length = 0.;
	printf("planView\n");
	for( i=0 ; i<size() ; i++ ) {
		/* Start an element named "geometry" as child of planView. */
		printf("  geometry\n");

		/* Add an attribute with name "s" and value "1.0" to ORDER. */
		printf("    s: %lf", total_length);

		odrGeometry geoNode = at(i);
		
		printf("    x,y: %lf, %lf\n", 	geoNode.x, geoNode.y);
		printf("    hdg: %lf\n",	geoNode.hdg);
		printf("    length: %lf\n", 	geoNode.length);

		assert( abs(total_length-geoNode.s)<0.001 );

		total_length += geoNode.length;
	}
	printf("listed %u segs\n",i);
}


int odRoad::loadXodr(string &fname) {

	return 0;
}

int odRoad::saveXodr(const char *fname) {
	xmlWriter xmlFile;
	xmlFile.open(fname);
	xmlFile.writeElement("OpenDRIVE");
	  xmlFile.writeElement("header");
	    xmlFile.writeAttribute("revMajor", "1");
	    xmlFile.writeAttribute("revMinor", "4");
	    xmlFile.writeAttribute("vendor", "ISEP");
	    xmlFile.writeElement("geoReference");
	    
	    xmlFile.closeElement();	// geoReference

	  xmlFile.closeElement();	// header
	  xmlFile.writeElement("road");
	    xmlFile.writeElement("link");
	  
	    xmlFile.closeElement();	// link
	    planView.saveXodr(xmlFile);
	  xmlFile.closeElement();	// road
	xmlFile.closeElement();		// OpenDRIVE
	xmlFile.close();
	return 0;
}

void odRoad::print() {
	planView.print();
};





