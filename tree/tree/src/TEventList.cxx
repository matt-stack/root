// @(#)root/tree:$Id$
// Author: Rene Brun   11/02/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

/** \class TEventList
A TEventList object is a list of selected events (entries) in a TTree.

A TEventList is automatically generated by TTree::Draw: example

    tree->Draw(">>elist1","x<0 && y> 0");

In this example, a TEventList object named "elist1" will be
generated. (Previous contents are overwritten).

    tree->Draw(">>+elist1","x<0 && y> 0");

In this example, selected entries are added to the list.

The TEventList object is added to the list of objects in the current
directory.

Use TTree:SetEventList(TEventList *list) to inform TTree that you
want to use the list as input. The following code gets a pointer to
the TEventList object created in the above commands:

    TEventList *list = (TEventList*)gDirectory->Get("elist1");

- Use function Enter to enter an element in the list
- The function Add may be used to merge two lists.
- The function Subtract may be used to subtract two lists.
- The function Reset may be used to reset a list.
- Use TEventList::Print(option) to print the contents.
     (option "all" prints all the list entries).
- Operators + and - correspond to functions Add and Subtract.
- A TEventList object can be saved on a file via the Write function.
*/

#include "TEventList.h"
#include "TCut.h"
#include "TClass.h"
#include "TFile.h"
#include "TMath.h"

ClassImp(TEventList)

////////////////////////////////////////////////////////////////////////////////
/// Default constructor for a EventList.

TEventList::TEventList(): TNamed()
{
   fN          = 0;
   fSize       = 100;
   fDelta      = 100;
   fList       = 0;
   fDirectory  = 0;
   fReapply    = kFALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// Create a EventList.
///
/// This Eventlist is added to the list of objects in current directory.

TEventList::TEventList(const char *name, const char *title, Int_t initsize, Int_t delta)
  :TNamed(name,title), fReapply(kFALSE)
{
   fN = 0;
   if (initsize > 100) fSize  = initsize;
   else                fSize  = 100;
   if (delta > 100)    fDelta = delta;
   else                fDelta = 100;
   fList       = 0;
   fDirectory  = gDirectory;
   if (fDirectory) fDirectory->Append(this);
}

////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.

TEventList::TEventList(const TEventList &list) : TNamed(list)
{
   fN     = list.fN;
   fSize  = list.fSize;
   fDelta = list.fDelta;
   fList  = new Long64_t[fSize];
   for (Int_t i=0; i<fN; i++)
      fList[i] = list.fList[i];
   fReapply = list.fReapply;
   fDirectory = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Default destructor for a EventList.

TEventList::~TEventList()
{
   delete [] fList;  fList = 0;
   if (fDirectory) fDirectory->Remove(this);
   fDirectory  = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Merge contents of alist with this list.
///
/// Both alist and this list are assumed to be sorted prior to this call

void TEventList::Add(const TEventList *alist)
{
   Int_t i;
   Int_t an = alist->GetN();
   if (!an) return;
   Long64_t *alst = alist->GetList();
   if (!fList) {
      fList = new Long64_t[an];
      for (i=0;i<an;i++) fList[i] = alst[i];
      fN = an;
      fSize = an;
      return;
   }
   Int_t newsize = fN + an;
   Long64_t *newlist = new Long64_t[newsize];
   Int_t newpos, alpos;
   newpos = alpos = 0;
   for (i=0;i<fN;i++) {
      while (alpos < an && fList[i] > alst[alpos]) {
         newlist[newpos] = alst[alpos];
         newpos++;
         alpos++;
      }
      if (alpos < an && fList[i] == alst[alpos]) alpos++;
      newlist[newpos] = fList[i];
      newpos++;
   }
   while (alpos < an) {
      newlist[newpos] = alst[alpos];
      newpos++;
      alpos++;
   }
   delete [] fList;
   fN    = newpos;
   fSize = newsize;
   fList = newlist;

   TCut orig = GetTitle();
   TCut added = alist->GetTitle();
   TCut updated = orig || added;
   SetTitle(updated.GetTitle());
}

////////////////////////////////////////////////////////////////////////////////
/// Return TRUE if list contains entry.

Bool_t TEventList::Contains(Long64_t entry)
{
   if (GetIndex(entry) < 0) return kFALSE;
   return kTRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// Return TRUE if list contains entries from entrymin to entrymax included.

Bool_t TEventList::ContainsRange(Long64_t entrymin, Long64_t entrymax)
{
   Long64_t imax = TMath::BinarySearch(fN,fList,entrymax);
   //printf("ContainsRange: entrymin=%lld, entrymax=%lld,imax=%lld, fList[imax]=%lld\n",entrymin,entrymax,imax,fList[imax]);

   if (fList[imax] < entrymin) return kFALSE;
   return kTRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// Called by TKey and others to automatically add us to a directory when we are read from a file.

void TEventList::DirectoryAutoAdd(TDirectory* dir)
{
   SetDirectory(dir);
}

////////////////////////////////////////////////////////////////////////////////
/// Enter element entry into the list.

void TEventList::Enter(Long64_t entry)
{
   if (!fList) {
      fList = new Long64_t[fSize];
      fList[0] = entry;
      fN = 1;
      return;
   }
   if (fN>0 && entry==fList[fN-1]) return;
   if (fN >= fSize) {
      Int_t newsize = TMath::Max(2*fSize,fN+fDelta);
      Resize(newsize-fSize);
   }
   if(fN==0 || entry>fList[fN-1]) {
      fList[fN] = entry;
      ++fN;
   } else {
      Int_t pos = TMath::BinarySearch(fN, fList, entry);
      if(pos>=0 && entry==fList[pos])
         return;
      ++pos;
      memmove( &(fList[pos+1]), &(fList[pos]), 8*(fN-pos));
      fList[pos] = entry;
      ++fN;
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Return value of entry at index in the list.
/// Return -1 if index is not in the list range.

Long64_t TEventList::GetEntry(Int_t index) const
{
   if (!fList)   return -1;
   if (index < 0 || index >= fN)   return -1;
   return fList[index];
}

////////////////////////////////////////////////////////////////////////////////
/// Return index in the list of element with value entry
/// array is supposed  to be sorted prior to this call.
/// If match is found, function returns position of element.
/// If no match found, function returns -1.

Int_t TEventList::GetIndex(Long64_t entry) const
{
   Long64_t nabove, nbelow, middle;
   nabove = fN+1;
   nbelow = 0;
   while(nabove-nbelow > 1) {
      middle = (nabove+nbelow)/2;
      if (entry == fList[middle-1]) return middle-1;
      if (entry  < fList[middle-1]) nabove = middle;
      else                          nbelow = middle;
   }
   return -1;
}

////////////////////////////////////////////////////////////////////////////////
/// Remove elements from this list that are NOT present in alist.

void TEventList::Intersect(const TEventList *alist)
{
   if (!alist) return;
   if (!fList) return;

   Long64_t *newlist = new Long64_t[fN];
   Int_t newpos = 0;
   Int_t i;
   for (i=0;i<fN;i++) {
      if (alist->GetIndex(fList[i]) >= 0) {
         newlist[newpos] = fList[i];
         newpos++;
      }
   }
   delete [] fList;
   fN    = newpos;
   fList = newlist;

   TCut orig = GetTitle();
   TCut removed = alist->GetTitle();
   TCut updated = orig && removed;
   SetTitle(updated.GetTitle());
}

////////////////////////////////////////////////////////////////////////////////
/// Merge entries in all the TEventList in the collection in this event list.

Int_t TEventList::Merge(TCollection *list)
{
   if (!list) return -1;
   TIter next(list);

   //first loop to count the number of entries
   TEventList *el;
   Int_t nevents = 0;
   while ((el = (TEventList*)next())) {
      if (!el->InheritsFrom(TEventList::Class())) {
         Error("Add","Attempt to add object of class: %s to a %s",el->ClassName(),this->ClassName());
         return -1;
      }
      Add(el);
      nevents += el->GetN();
   }

   return nevents;
}

////////////////////////////////////////////////////////////////////////////////
/// Print contents of this list.

void TEventList::Print(Option_t *option) const
{
   printf("EventList:%s/%s, number of entries =%d, size=%d\n",GetName(),GetTitle(),fN,fSize);
   if (!strstr(option,"all")) return;
   Int_t i;
   Int_t nbuf = 0;
   char element[10];
   char *line = new char[100];
   snprintf(line,100,"%5d : ",0);
   for (i=0;i<fN;i++) {
      nbuf++;
      if (nbuf > 10) {
         printf("%s\n",line);
         snprintf(line,100,"%5d : ",i);
         nbuf = 1;
      }
      snprintf(element,10,"%7lld ",fList[i]);
      strlcat(line,element,100);
   }
   if (nbuf) printf("%s\n",line);
   delete [] line;
}

////////////////////////////////////////////////////////////////////////////////
/// Reset number of entries in event list.

void TEventList::Reset(Option_t *)
{
   fN = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Resize list by delta entries.

void TEventList::Resize(Int_t delta)
{
   if (!delta) delta = fDelta;
   fSize += delta;
   Long64_t *newlist = new Long64_t[fSize];
   for (Int_t i=0;i<fN;i++) newlist[i] = fList[i];
   delete [] fList;
   fList = newlist;
}

////////////////////////////////////////////////////////////////////////////////
/// Remove reference to this EventList from current directory and add
/// reference to new directory dir. dir can be 0 in which case the list
/// does not belong to any directory.

void TEventList::SetDirectory(TDirectory *dir)
{
   if (fDirectory == dir) return;
   if (fDirectory) fDirectory->Remove(this);
   fDirectory = dir;
   if (fDirectory) fDirectory->Append(this);
}

////////////////////////////////////////////////////////////////////////////////
/// Change the name of this TEventList.

void TEventList::SetName(const char *name)
{
   //  TEventLists are named objects in a THashList.
   //  We must update the hashlist if we change the name
   if (fDirectory) fDirectory->Remove(this);
   fName = name;
   if (fDirectory) fDirectory->Append(this);
}

////////////////////////////////////////////////////////////////////////////////
/// Sort list entries in increasing order

void TEventList::Sort()
{
   Int_t    *index   = new Int_t[fN];
   Long64_t *newlist = new Long64_t[fSize];
   Int_t i,ind;
   TMath::Sort(fN,fList,index); //sort in decreasing order
   for (i=0;i<fN;i++) {
      ind = index[fN-i-1];
      newlist[i] = fList[ind];
   }
   for (i=fN;i<fSize;i++) {
      newlist[i] = 0;
   }
   delete [] index;
   delete [] fList;
   fList = newlist;
}

////////////////////////////////////////////////////////////////////////////////
/// Stream an object of class TEventList.

void TEventList::Streamer(TBuffer &b)
{
   if (b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = b.ReadVersion(&R__s, &R__c);
      fDirectory = 0;
      if (R__v > 1) {
         b.ReadClassBuffer(TEventList::Class(), this, R__v, R__s, R__c);
         ResetBit(kMustCleanup);
         return;
      }
      //====process old versions before automatic schema evolution
      TNamed::Streamer(b);
      b >> fN;
      b >> fSize;
      b >> fDelta;
      if (fN) {
         Int_t *tlist = new Int_t[fSize];
         b.ReadFastArray(tlist,fN);
         fList = new Long64_t[fSize];
         for (Int_t i=0;i<fN;i++) fList[i] = tlist[i];
         delete [] tlist;
      }
      ResetBit(kMustCleanup);
      b.CheckByteCount(R__s, R__c, TEventList::IsA());
      //====end of old versions

   } else {
      b.WriteClassBuffer(TEventList::Class(), this);
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Remove elements from this list that are present in alist.

void TEventList::Subtract(const TEventList *alist)
{
   if (!alist) return;
   if (!fList) return;

   Long64_t *newlist = new Long64_t[fN];
   Int_t newpos = 0;
   Int_t i;
   for (i=0;i<fN;i++) {
      if (alist->GetIndex(fList[i]) < 0) {
         newlist[newpos] = fList[i];
         newpos++;
      }
   }
   delete [] fList;
   fN    = newpos;
   fList = newlist;

   TCut orig = GetTitle();
   TCut removed = alist->GetTitle();
   TCut updated = orig && !removed;
   SetTitle(updated.GetTitle());
}

////////////////////////////////////////////////////////////////////////////////
/// Assingment.

TEventList& TEventList::operator=(const TEventList &list)
{
   if (this != &list) {
      TNamed::operator=(list);
      if (fSize < list.fSize) {
         delete [] fList;
         fList  = new Long64_t[list.fSize];
      }
      fN     = list.fN;
      fSize  = list.fSize;
      fDelta = list.fDelta;
      for (Int_t i=0; i<fN; i++)
         fList[i] = list.fList[i];
   }
   return *this;
}

////////////////////////////////////////////////////////////////////////////////
/// Addition.

TEventList operator+(const TEventList &list1, const TEventList &list2)
{
   TEventList newlist = list1;
   newlist.Add(&list2);
   return newlist;
}

////////////////////////////////////////////////////////////////////////////////
/// Substraction

TEventList operator-(const TEventList &list1, const TEventList &list2)
{
   TEventList newlist = list1;
   newlist.Subtract(&list2);
   return newlist;
}

////////////////////////////////////////////////////////////////////////////////
/// Intersection.

TEventList operator*(const TEventList &list1, const TEventList &list2)
{
   TEventList newlist = list1;
   newlist.Intersect(&list2);
   return newlist;
}

