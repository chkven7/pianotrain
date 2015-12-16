
#ifdef WIN32
# pragma warning (disable : 4786)
#endif

#include <algorithm>
#include <iostream>

#include "piano.h"
#include <string.h>
#include <stdio.h>
#include <io.h>

#include "elements.h"
#include "factory.h"
#include "xml.h"
#include "xmlfile.h"
#include "xmlreader.h"
#include "xml_tree_browser.h"
#include "unrolled_xml_tree_browser.h"

using namespace std;
using namespace MusicXML2;

void partstart(char *);
void partend(char *);
void measurestart(char *);
void measureend(char *);
void notestart(void);
void noteend(void);
void chord(void);
void pitch(char *,char *,char *);
//void notevalue(char *);
//void alteracion(char *);
void tie(char *);
void figura(char *);
void staff(char *);
void armadura(char *);
void numerador(char *);
void denominador(char *);
void staves(char *);

class measurevisitor : 
    public visitor<S_beats>,
    public visitor<S_beat_type>,
	public visitor<S_staves>,
    public visitor<S_fifths>,
    public visitor<S_staff>,
    public visitor<S_type>,
    public visitor<S_tie>,
    public visitor<S_octave>,
    public visitor<S_alter>,
    public visitor<S_step>,
    public visitor<S_pitch>,
	public visitor<S_chord>,
    public visitor<S_note>,
	public visitor<S_measure>, 
	public visitor<S_part>
{
    public:
				 measurevisitor() {}
       	virtual ~measurevisitor() {}

		virtual void visitStart( S_part& elt ) {
			char arr[256];
			cout << "Start part " << elt->getAttributeValue("id") << endl;
			copy(elt->getAttributeValue("id").begin(),elt->getAttributeValue("id").end(),arr);
			partstart(arr);
		}
		virtual void visitEnd( S_part& elt ) {
			char arr[256];
			cout << "End part " << elt->getAttributeValue("id") << endl;
			copy(elt->getAttributeValue("id").begin(),elt->getAttributeValue("id").end(),arr);
			partend(arr);
		}
		virtual void visitStart( S_measure& elt ) {
			char arr[256];
			cout << "Start measure " << elt->getAttributeValue("number") << endl;
            copy(elt->getAttributeValue("number").begin(),elt->getAttributeValue("number").end(),arr);            
			measurestart(arr);
		}
		virtual void visitEnd( S_measure& elt ) {
			char arr[256];
			cout << "End measure " << elt->getAttributeValue("number") << endl;
            copy(elt->getAttributeValue("number").begin(),elt->getAttributeValue("number").end(),arr);            
			measureend(arr);
		}
		virtual void visitStart( S_note& elt ) {
			cout << "Start note " << endl;
            notestart();
		}
		virtual void visitEnd( S_note& elt ) {
			cout << "End note " << endl;
			noteend();
		}
		virtual void visitStart( S_chord& elt ) {
			cout << "chord: " << endl;
			chord();
		}
		virtual void visitStart( S_pitch& elt ) {
			cout << "pitch " <<  endl;
//			pitch(elt->getValue(k_note),elt->getValue(k_alter),elt->getValue(k_octave));
		}
		virtual void visitStart( S_step& elt ) {
			char arr[256];
			cout << "step: " << elt->getValue() << endl;
//            copy(elt->getValue().begin(),elt->getValue().end(),arr);            
//			notevalue(arr);
		}
		virtual void visitStart( S_alter& elt ) {
			char arr[256];
			cout << "alter: " << elt->getValue() << endl;
//			copy(elt->getValue().begin(),elt->getValue().end(),arr);            
//			alteracion(arr);
		}

		virtual void visitStart( S_tie& elt ) {
			char arr[256];
			cout << "tie: " << elt->getAttributeValue("type") << endl;
			copy(elt->getAttributeValue("type").begin(),elt->getAttributeValue("type").end(),arr);
			tie(arr);
		}
		virtual void visitStart( S_type& elt ) {
			char arr[256];
			cout << "tipo_figura: " << elt->getValue() << endl;
			copy(elt->getValue().begin(),elt->getValue().end(),arr);            
			figura(arr);
		} 
		virtual void visitStart( S_staff& elt ) {
			char arr[256];
			cout << "staff: " << elt->getValue() << endl;
			copy(elt->getValue().begin(),elt->getValue().end(),arr);            
			staff(arr);
		}
		virtual void visitStart( S_fifths& elt ) {
			char arr[256];
			cout << "armadura: " << elt->getValue() << endl;
			copy(elt->getValue().begin(),elt->getValue().end(),arr);            
			armadura(arr);
		}
		virtual void visitStart( S_beats& elt ) {
			char arr[256];
			cout << "Numerador: " << elt->getValue() << endl;
			copy(elt->getValue().begin(),elt->getValue().end(),arr);            
			numerador(arr);
		}
		virtual void visitStart( S_beat_type& elt ) {
			char arr[256];
			cout << "Denominador: " << elt->getValue() << endl;
			copy(elt->getValue().begin(),elt->getValue().end(),arr);            
			denominador(arr);
		}
		virtual void visitStart( S_staves& elt ) {
			char arr[256];
			cout << "Staves: " << elt->getValue() << endl;
			copy(elt->getValue().begin(),elt->getValue().end(),arr);            
			staves(arr);
		}
};


int parsexml(char *filename)
{
	xmlreader r;  
	SXMLFile file = r.read(filename);   
	if (file) {
		Sxmlelement elts = file->elements();
		measurevisitor mv;
		xml_tree_browser tb(&mv);
		tb.browse (*elts);
	}
	return 0;
}

/*************************************************************
int loadxml(MELODIA *melod,char *filename)
{
	xmlreader r;  
	SXMLFile file = r.read(filename);   
	if (file) {
		Sxmlelement elts = file->elements();
		measurevisitor mv;
		cout << ">>>>>>>>>>>> Rolled score <<<<<<<<<<<<" << endl;
		xml_tree_browser tb(&mv);
		tb.browse (*elts);
		cout << ">>>>>>>>>>>> Unrolled score <<<<<<<<<<<<" << endl;
		unrolled_xml_tree_browser utb(&mv);
		utb.browse (*elts);
	}
	return 0;
}


	/*
class predicate {
	public:
	   int fType;

	   predicate(int type) : fType(type) {}
	   virtual ~predicate() {}
	   virtual bool operator () (const Sxmlelement elt) const { 
	      return elt->getType() == fType;
	   }
};

int countmeasures_(SXMLFile file)
{
   int val;
 
   Sxmlelement elt = file->elements();
   predicate p(k_measure);
   val=count_if(elt->begin(), elt->end(), p);
   return val;
}

int countparts_(SXMLFile file)
{
   int val;
 
   Sxmlelement elt = file->elements();
   predicate p(k_part);
   val=count_if(elt->begin(), elt->end(), p);
   return val;
}

int countmeasures(char *filename)
{
   int val;
   xmlreader r;  
   SXMLFile file = r.read(filename);   
   val=countmeasures_(file);
   return val;
}

int countparts(char *filename)
{
   int val;
   xmlreader r;  
   SXMLFile file = r.read(filename);   
   val=countparts_(file);
   return val;
}
*/






