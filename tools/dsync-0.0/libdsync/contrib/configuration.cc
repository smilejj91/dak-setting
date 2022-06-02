// -*- mode: cpp; mode: fold -*-
// Description								/*{{{*/
// $Id: configuration.cc,v 1.5 1999/11/17 05:59:29 jgg Exp $
/* ######################################################################

   Configuration Class
   
   This class provides a configuration file and command line parser
   for a tree-oriented configuration environment. All runtime configuration
   is stored in here.
   
   ##################################################################### */
									/*}}}*/
// Include files							/*{{{*/
#ifdef __GNUG__
#pragma implementation "dsync/configuration.h"
#endif
#include <dsync/configuration.h>
#include <dsync/error.h>
#include <dsync/strutl.h>

#include <stdio.h>
#include <fstream>
#include <iostream>
using namespace std;
									/*}}}*/

Configuration *_config = new Configuration;

// Configuration::Configuration - Constructor				/*{{{*/
// ---------------------------------------------------------------------
/* */
Configuration::Configuration()
{
   Root = new Item;
}
									/*}}}*/
// Configuration::Lookup - Lookup a single item				/*{{{*/
// ---------------------------------------------------------------------
/* This will lookup a single item by name below another item. It is a 
   helper function for the main lookup function */
Configuration::Item *Configuration::Lookup(Item *Head,const char *S,
					   unsigned long Len,bool Create)
{
   int Res = 1;
   Item *I = Head->Child;
   Item **Last = &Head->Child;
   
   // Empty strings match nothing. They are used for lists.
   if (Len != 0)
   {
      for (; I != 0; Last = &I->Next, I = I->Next)
	 if ((Res = stringcasecmp(I->Tag.c_str(), I->Tag.c_str() + strlen(I->Tag.c_str()),S,S + Len)) == 0)
	    break;
   }
   else
      for (; I != 0; Last = &I->Next, I = I->Next);
      
   if (Res == 0)
      return I;
   if (Create == false)
      return 0;
   
   I = new Item;
   I->Tag = string(S,Len);
   I->Next = *Last;
   I->Parent = Head;
   *Last = I;
   return I;
}
									/*}}}*/
// Configuration::Lookup - Lookup a fully scoped item			/*{{{*/
// ---------------------------------------------------------------------
/* This performs a fully scoped lookup of a given name, possibly creating
   new items */
Configuration::Item *Configuration::Lookup(const char *Name,bool Create)
{
   if (Name == 0)
      return Root->Child;
   
   const char *Start = Name;
   const char *End = Start + strlen(Name);
   const char *TagEnd = Name;
   Item *Itm = Root;
   for (; End - TagEnd >= 2; TagEnd++)
   {
      if (TagEnd[0] == ':' && TagEnd[1] == ':')
      {
	 Itm = Lookup(Itm,Start,TagEnd - Start,Create);
	 if (Itm == 0)
	    return 0;
	 TagEnd = Start = TagEnd + 2;	 
      }
   }   

   // This must be a trailing ::, we create unique items in a list
   if (End - Start == 0)
   {
      if (Create == false)
	 return 0;
   }
   
   Itm = Lookup(Itm,Start,End - Start,Create);
   return Itm;
}
									/*}}}*/
// Configuration::Find - Find a value					/*{{{*/
// ---------------------------------------------------------------------
/* */
string Configuration::Find(const char *Name,const char *Default)
{
   Item *Itm = Lookup(Name,false);
   if (Itm == 0 || Itm->Value.empty() == true)
   {
      if (Default == 0)
	 return string();
      else
	 return Default;
   }
   
   return Itm->Value;
}
									/*}}}*/
// Configuration::FindFile - Find a Filename				/*{{{*/
// ---------------------------------------------------------------------
/* Directories are stored as the base dir in the Parent node and the
   sub directory in sub nodes with the final node being the end filename
 */
string Configuration::FindFile(const char *Name,const char *Default)
{
   Item *Itm = Lookup(Name,false);
   if (Itm == 0 || Itm->Value.empty() == true)
   {
      if (Default == 0)
	 return string();
      else
	 return Default;
   }
   
   // Absolute path
   if (Itm->Value[0] == '/' || Itm->Parent == 0)
      return Itm->Value;
   
   // ./ is also considered absolute as is anything with ~ in it
   if (Itm->Value[0] != 0 && 
       ((Itm->Value[0] == '.' && Itm->Value[1] == '/') ||
	(Itm->Value[0] == '~' && Itm->Value[1] == '/')))
      return Itm->Value;
   
   if (Itm->Parent->Value.end()[-1] == '/')
      return Itm->Parent->Value + Itm->Value;
   else
      return Itm->Parent->Value + '/' + Itm->Value;
}
									/*}}}*/
// Configuration::FindDir - Find a directory name			/*{{{*/
// ---------------------------------------------------------------------
/* This is like findfile execept the result is terminated in a / */
string Configuration::FindDir(const char *Name,const char *Default)
{
   string Res = FindFile(Name,Default);
   if (Res.end()[-1] != '/')
      return Res + '/';
   return Res;
}
									/*}}}*/
// Configuration::FindI - Find an integer value				/*{{{*/
// ---------------------------------------------------------------------
/* */
int Configuration::FindI(const char *Name,int Default)
{
   Item *Itm = Lookup(Name,false);
   if (Itm == 0 || Itm->Value.empty() == true)
      return Default;
   
   char *End;
   int Res = strtol(Itm->Value.c_str(),&End,0);
   if (End == Itm->Value.c_str())
      return Default;
   
   return Res;
}
									/*}}}*/
// Configuration::FindB - Find a boolean type				/*{{{*/
// ---------------------------------------------------------------------
/* */
bool Configuration::FindB(const char *Name,bool Default)
{
   Item *Itm = Lookup(Name,false);
   if (Itm == 0 || Itm->Value.empty() == true)
      return Default;
   
   return StringToBool(Itm->Value,Default);
}
									/*}}}*/
// Configuration::Set - Set a value					/*{{{*/
// ---------------------------------------------------------------------
/* */
void Configuration::Set(const char *Name,string Value)
{
   Item *Itm = Lookup(Name,true);
   if (Itm == 0)
      return;
   Itm->Value = Value;
}
									/*}}}*/
// Configuration::Set - Set an integer value				/*{{{*/
// ---------------------------------------------------------------------
/* */
void Configuration::Set(const char *Name,int Value)
{
   Item *Itm = Lookup(Name,true);
   if (Itm == 0)
      return;
   char S[300];
   snprintf(S,sizeof(S),"%i",Value);
   Itm->Value = S;
}
									/*}}}*/
// Configuration::Exists - Returns true if the Name exists		/*{{{*/
// ---------------------------------------------------------------------
/* */
bool Configuration::Exists(const char *Name)
{
   Item *Itm = Lookup(Name,false);
   if (Itm == 0)
      return false;
   return true;
}
									/*}}}*/
// Configuration::Dump - Dump the config				/*{{{*/
// ---------------------------------------------------------------------
/* Dump the entire configuration space */
void Configuration::Dump()
{
   /* Write out all of the configuration directives by walking the 
      configuration tree */
   const Configuration::Item *Top = _config->Tree(0);
   for (; Top != 0;)
   {
      clog << Top->FullTag() << " \"" << Top->Value << "\";" << endl;
      
      if (Top->Child != 0)
      {
	 Top = Top->Child;
	 continue;
      }
      
      while (Top != 0 && Top->Next == 0)
	 Top = Top->Parent;
      if (Top != 0)
	 Top = Top->Next;
   }   
}
									/*}}}*/

// Configuration::Item::FullTag - Return the fully scoped tag		/*{{{*/
// ---------------------------------------------------------------------
/* */
string Configuration::Item::FullTag() const
{
   if (Parent == 0 || Parent->Parent == 0)
      return Tag;
   return Parent->FullTag() + "::" + Tag;
}
									/*}}}*/

// ReadConfigFile - Read a configuration file				/*{{{*/
// ---------------------------------------------------------------------
/* The configuration format is very much like the named.conf format
   used in bind8, in fact this routine can parse most named.conf files. */
bool ReadConfigFile(Configuration &Conf,string FName)
{
   // Open the stream for reading
   ifstream F(FName.c_str(),ios::in);
   if (!F != 0)
      return _error->Errno("ifstream::ifstream","Opening configuration file %s",FName.c_str());
   
   char Buffer[300];
   string LineBuffer;
   string Stack[100];
   unsigned int StackPos = 0;
   
   // Parser state
   string ParentTag;
   
   int CurLine = 0;
   bool InComment = false;
   while (F.eof() == false)
   {
      F.getline(Buffer,sizeof(Buffer));
      CurLine++;
      _strtabexpand(Buffer,sizeof(Buffer));
      _strstrip(Buffer);

      // Multi line comment
      if (InComment == true)
      {
	 for (const char *I = Buffer; *I != 0; I++)
	 {
	    if (*I == '*' && I[1] == '/')
	    {
	       memmove(Buffer,I+2,strlen(I+2) + 1);
	       InComment = false;
	       break;
	    }	    
	 }
	 if (InComment == true)
	    continue;
      }
      
      // Discard single line comments
      bool InQuote = false;
      for (char *I = Buffer; *I != 0; I++)
      {
	 if (*I == '"')
	    InQuote = !InQuote;
	 if (InQuote == true)
	    continue;
	 
	 if (*I == '/' && I[1] == '/')
         {
	    *I = 0;
	    break;
	 }
      }
      
      // Look for multi line comments
      for (char *I = Buffer; *I != 0; I++)
      {
	 if (*I == '"')
	    InQuote = !InQuote;
	 if (InQuote == true)
	    continue;
	 
	 if (*I == '/' && I[1] == '*')
         {
	    InComment = true;
	    for (char *J = Buffer; *J != 0; J++)
	    {
	       if (*J == '*' && J[1] == '/')
	       {
		  memmove(I,J+2,strlen(J+2) + 1);
		  InComment = false;
		  break;
	       }	       
	    }
	    
	    if (InComment == true)
	    {
	       *I = 0;
	       break;
	    }	    
	 }
      }
      
      // Blank
      if (Buffer[0] == 0)
	 continue;
      
      // We now have a valid line fragment
      for (char *I = Buffer; *I != 0;)
      {
	 if (*I == '{' || *I == ';' || *I == '}')
	 {
	    // Put the last fragement into the buffer
	    char *Start = Buffer;
	    char *Stop = I;
	    for (; Start != I && isspace(*Start) != 0; Start++);
	    for (; Stop != Start && isspace(Stop[-1]) != 0; Stop--);
	    if (LineBuffer.empty() == false && Stop - Start != 0)
	       LineBuffer += ' ';
	    LineBuffer += string(Start,Stop - Start);
	    
	    // Remove the fragment
	    char TermChar = *I;
	    memmove(Buffer,I + 1,strlen(I + 1) + 1);
	    I = Buffer;

	    // Move up a tag
	    if (TermChar == '}')
	    {
	       if (StackPos == 0)
		  ParentTag = string();
	       else
		  ParentTag = Stack[--StackPos];
	    }
	    
	    // Syntax Error
	    if (TermChar == '{' && LineBuffer.empty() == true)
	       return _error->Error("Syntax error %s:%u: Block starts with no name.",FName.c_str(),CurLine);
	    
	    if (LineBuffer.empty() == true)
	       continue;

	    // Parse off the tag
	    string Tag;
	    const char *Pos = LineBuffer.c_str();
	    if (ParseQuoteWord(Pos,Tag) == false)
	       return _error->Error("Syntax error %s:%u: Malformed Tag",FName.c_str(),CurLine);	    
	    
	    // Go down a level
	    if (TermChar == '{')
	    {
	       if (StackPos <= 100)
		  Stack[StackPos++] = ParentTag;
	       if (ParentTag.empty() == true)
		  ParentTag = Tag;
	       else
		  ParentTag += string("::") + Tag;
	       Tag = string();
	    }

	    // Parse off the word
	    string Word;
	    if (ParseCWord(Pos,Word) == false)
	    {
	       if (TermChar != '{')
	       {
		  Word = Tag;
		  Tag = "";
	       }	       
	    }
	    
	    // Generate the item name
	    string Item;
	    if (ParentTag.empty() == true)
	       Item = Tag;
	    else
	    {
	       if (TermChar != '{' || Tag.empty() == false)
		  Item = ParentTag + "::" + Tag;
	       else
		  Item = ParentTag;
	    }
	    
	    // Set the item in the configuration class
	    Conf.Set(Item,Word);
			    
	    // Empty the buffer
	    LineBuffer = string();
	 }
	 else
	    I++;
      }

      // Store the fragment
      const char *Stripd = _strstrip(Buffer);
      if (*Stripd != 0 && LineBuffer.empty() == false)
	 LineBuffer += " ";
      LineBuffer += Stripd;
   }
   
   return true;
}
									/*}}}*/
