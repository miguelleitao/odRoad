/*
 * odRoad
 *
 */

#include <math.h>
#include <cassert>
#include "odRoad.h"

int verbose = 0;

void CopyVecD(scalar *v, scalar *dest) {
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

	scalar seg_length = 0.;
	int file_format = 6;
	scalar p[3], e[3];
	//double last_v[3];
	scalar last_p[3] = {0.,0.,0.};
	//double v[3];
	char *line = (char *) malloc (PTS_MAX_LINE_SIZE + 1);
	size_t line_len = PTS_MAX_LINE_SIZE;

	length = 0.;
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
			res = sscanf(line,"%Lf %Lf %Lf %Lf %Lf %Lf\n", p+0, p+1, p+2, e+0, e+1, e+2);
			lines++;
			if ( verbose>=4 ) {
				printf("res %d, %Lf %Lf %Lf\n",res,p[0],p[1],p[2]);
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
			if ( sscanf(line,"%Lf", p+1) <1 ) {
				invalid++;
				continue;
			}
			p[0] = (double)vertexes;
			vertexes++;
		}
		seg_length = sqrtl( (p[0]-last_p[0])*(p[0]-last_p[0]) + (p[1]-last_p[1])*(p[1]-last_p[1]) );
		e[0] = atan2l( p[1]-last_p[1], p[0]-last_p[0]);

		odrGeometry *geoNode = new odrLine();
		geoNode->s = length;
		geoNode->x = p[0];
		geoNode->y = p[1];
		geoNode->hdg = e[0];
		geoNode->length = seg_length;
		push_back(*geoNode);
		
		length += seg_length;

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
		rc = xmlFile.writeAttributeScalar("s", total_length);

		odrGeometry geoNode = at(i);
		
		xmlFile.writeAttributesXY( geoNode.x, geoNode.y);
		xmlFile.writeAttributeScalar("hdg", geoNode.hdg);
		xmlFile.writeAttributeScalar("length", geoNode.length);

		/* Start an element named "line" as child of geometry. */
		rc = xmlFile.writeElement("line");
	
		/* Close the element named line. */
		rc = xmlFile.closeElement();

		assert( abs(total_length-geoNode.s)<0.001 );

		total_length += geoNode.length;
		/* Close the element named geometry. */
		rc = xmlFile.closeElement();
	}
	/* Close the element named planView. */
	rc = xmlFile.closeElement();
	printf("gravou %u segs\n",i);
	return i;
}


int PlanView::savePts(const char* fname) {
	unsigned i;

	FILE *fout;
	fout = fopen(fname,"w");
	if ( ! fout ) {
		fprintf(stderr,"Error: Cannot open fle '%s' in write mode.\n", fname);
		return -1;
	}
	fprintf(fout,"#pts_v3\n");
	fprintf(fout,"#lines:%ld\n", size());
	fprintf(fout,"#columns:%d\n", 6);	
	fprintf(fout,"#segtype:%d\n", 0);

	double total_length = 0.;
	for( i=0 ; i<size() ; i++ ) {
		odrGeometry geoNode = at(i);
		
		printf("%Lf %Lf %Lf %Lf %Lf %Lf\n", geoNode.x, geoNode.y, 0.0L, geoNode.hdg, 0.0L, 0.0L);
		assert( abs(total_length-geoNode.s)<0.001 );
		total_length += geoNode.length;
	}
	return size();
}

void PlanView::print() {
	unsigned i;
	
	double total_length = 0.;
	printf("planView\n");
	for( i=0 ; i<size() ; i++ ) {
		/* Start an element named "geometry". */
		printf("  geometry\n");

		/* Add an attribute with name "s". */
		printf("    s: %lf", total_length);

		odrGeometry geoNode = at(i);
		
		printf("    x,y: %.3lf, %lf\n", 	double(geoNode.x), double(geoNode.y) );
		printf("    hdg: %.3lf\n",	double(geoNode.hdg));
		printf("    length: %.3Lf\n", 	geoNode.length);

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
	    xmlFile.writeAttribute("name", name);
	    xmlFile.writeAttributeScalar("length", length);
	    xmlFile.writeAttributeInt("id", id);
	    xmlFile.writeAttributeInt("junction", junction);

	    xmlFile.writeElement("link");
	    xmlFile.closeElement();	// link

	    planView.saveXodr(xmlFile);

	    xmlFile.writeElement("elevationProfile");
	    xmlFile.closeElement();	// elevationProfile

	    xmlFile.writeElement("lateralProfile");
	    xmlFile.closeElement();	// lateralProfile

	    lanes.saveXodr(xmlFile);

	  xmlFile.closeElement();	// road
	xmlFile.closeElement();		// OpenDRIVE
	xmlFile.close();
	return 0;
}

void odRoad::print() {
	planView.print();
	lanes.print();
};





