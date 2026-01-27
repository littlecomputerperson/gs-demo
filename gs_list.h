/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_list.cpp, gs_list.h                                                              |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_List                                                                             |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: A list class template allowing one to create a list of just about any data type.    |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_LIST_H
#define GS_LIST_H


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Include standard C library header files.
// ---------------------------------------------------------------------------------------------
#include <string.h>
#include <assert.h>
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


template<class type>
class GS_List
{

private:

    type* m_ptArray;     // A pointer to an array of items of any type.

    int m_nNumItems;     // Number of items actually in the array.
    int m_nNumAllocated; // Number of items for which space has been allocated.

    void Allocate(int nNumToAllocate);

public:

    GS_List(int size = 0);
    virtual ~GS_List();

    int FindItem(const type ctData);

    void AddItem(type tData);
    void InsertItem(int nIndex, const type ctData);

    void RemoveItem(const type tData);
    void RemoveIndex(int nIndex);
    void RemoveEmptyItems();
    void RemoveAllItems();

    inline type operator [] (int nIndex) const;
    inline type GetItem(int nIndex);
    void SetItem(int nIndex, type tData);

    inline int GetNumItems();
    void SetNumItems(int nNumToAllocate);

    inline BOOL IsEmpty();
};


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::GS_List():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
GS_List<type>::GS_List(int nNumToAllocate)
{

    m_ptArray       = 0;
    m_nNumItems     = 0;
    m_nNumAllocated = 0;

    if (nNumToAllocate)
    {
        SetNumItems(nNumToAllocate);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::~GS_List():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
GS_List<type>::~GS_List()
{

    if (m_ptArray)
    {
        delete [] m_ptArray;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Memory Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::Allocate():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::Allocate(int nNumToAllocate)
{

    // Can't alocate space for less items than currently in the list.
    if ((nNumToAllocate <= 0) || (nNumToAllocate < m_nNumItems))
    {
        return;
    }

    // Create a pointer to the current item array.
    type* ptOldArray = m_ptArray;

    // Set how many items to allocate space for.
    m_nNumAllocated = nNumToAllocate;

    // Allocate memory for a new array with space for m_nNumAllocated items.
    m_ptArray = new type[m_nNumAllocated];

    // If the old array had one or more items, copy them to the new array.
    if (m_nNumItems > 0)
    {
        memcpy(m_ptArray, ptOldArray, m_nNumItems * sizeof(type));
    }

    // Delete the old array.
    if (ptOldArray)
    {
        delete [] ptOldArray;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Find Methods. ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::GS_List():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
int GS_List<type>::FindItem(const type tData)
{

    // Loop through all the items in the list.
    for (int nLoop = 0; nLoop < m_nNumItems; nLoop++)
    {
        // Return the index if the item that matches the search criteria.
        if (m_ptArray[nLoop] == tData)
        {
            return nLoop;
        }
    }

    // Return -1 if no item was found.
    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Add Methods. ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::AddItem():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::AddItem(type tData)
{

    // If there are as many items as space have been allocated for.
    if (m_nNumItems == m_nNumAllocated)
    {
        // If no space has been allocated yet.
        if (m_nNumAllocated == 0)
        {
            // Allocate space for one item.
            Allocate(1);
        }
        else
        {
            // Allocate space for one more item.
            Allocate(m_nNumAllocated + 1);
        }
    }

    // Add the item to the end of the list.
    m_ptArray[m_nNumItems++] = tData;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::InsertItem():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::InsertItem(int nIndex, type tData)
{

    // Check if a valid index was specified.
    if ((nIndex < 0) || (nIndex > m_nNumItems))
    {
        return;
    }

    // If item is inserted at the end of the list.
    if ((m_nNumItems == 0) || (nIndex == m_nNumItems))
    {
        // Simply add the item to the list.
        addItem(tData);
    }
    else
    {
        // Save the last item in the list.
        type tLastItem = m_ptArray[m_nNumItems - 1];

        // Iterate backwards through the list up to where the new item will be inserted.
        for (int nLoop = m_nNumItems - 1; nLoop > nIndex; nLoop--)
        {
            // Overwite the current item in the list with the one just before it (thus moving all
            // the items, from the position where the new item should be inserted onward, one
            // position forward and overwriting the last item).
            m_ptArray[nLoop] = m_ptArray[nLoop - 1];
        }

        // Insert the item at the specified position.
        m_ptArray[nIndex] = tData;

        // Add the last item at the end of the list.
        AddItem(tLastItem);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Remove Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::RemoveItem():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::RemoveItem(const type tData)
{

    // Find the item to remove from the list.
    int nIndex = FindItem(tData);

    // Remove all items matching the criteria from the list.
    while (nIndex != -1)
    {
        RemoveIndex(nIndex);
        nIndex = FindItem(tData);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::RemoveIndex():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::RemoveIndex(int nIndex)
{


    // Check if a valid index was specified.
    if ((nIndex < 0) || (nIndex >= m_nNumItems))
    {
        return;
    }

    // Remove the item at the specified index.
    if (nIndex < m_nNumItems - 1)
    {
        // Move all the items after the indicated item one back to overwrite the indicated item.
        memmove(&m_ptArray[nIndex], &m_ptArray[nIndex+1], (m_nNumItems-1-nIndex) * sizeof(type));
    }

    // List now has one less item.
    m_nNumItems--;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::RemoveEmptyItems:
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::RemoveEmptyItems()
{

    int nIndex = m_nNumItems;

    // Iterate backwards through the list.
    while (--nIndex >= 0)
    {
        // If the indexed item is empty, remove it from the list.
        if (m_ptArray[nIndex] == 0)
        {
            RemoveIndex(nIndex);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::RemoveAllItems():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::RemoveAllItems()
{

    // Reset the number of items in the list to zero.
    m_nNumItems = 0;

    // If memory has been allocated for the array.
    if (m_ptArray)
    {
        // Delete the entire array.
        delete [] m_ptArray;
        m_ptArray = 0;
    }

    // Indicate that no memory has been allocated.
    m_nNumAllocated = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Get/Set Methods. ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::operator [] ():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
inline type GS_List<type>::operator [] (int nIndex) const
{

    // Check if a valid index was specified.
    assert((nIndex >= 0) && (nIndex < m_nNumItems));

    // Return the item at the specified index.
    return m_ptArray[nIndex];
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::GetItem():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An item of type at the specified index.
//==============================================================================================

template<class type>
inline type GS_List<type>::GetItem(int nIndex)
{

    // Check if a valid index was specified.
    assert((nIndex >= 0) && (nIndex < m_nNumItems));

    // Return the item at the specified index.
    return m_ptArray[nIndex];
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::SetItem():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::SetItem(int nIndex, type tData)
{

    // If the index is valid, set the item at the specified index to the specified data,
    // overwriting the previous item stored there, if any.
    if ((nIndex >= 0) || (nIndex < m_nNumItems))
    {
        m_ptArray[nIndex] = tData;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::GetNumItems():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
inline int GS_List<type>::GetNumItems()
{

    return m_nNumItems;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::SetNumItems():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

template<class type>
void GS_List<type>::SetNumItems(int nNumToAllocate)
{

    // If no space should be allocated.
    if (nNumToAllocate == 0)
    {
        // Delete the array.
        if (m_ptArray)
        {
            delete [] m_ptArray;
            m_ptArray = 0;
        }
    }
    else
    {
        // Allocate space for the required number of items.
        Allocate(nNumToAllocate);
    }


    // Set number of items to the number allocated.
    m_nNumItems = nNumToAllocate;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Status Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_List::IsEmpty():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if list is empty, FALSE if not.
//==============================================================================================

template<class type>
inline BOOL GS_List<type>::IsEmpty()
{

    return (m_nNumItems == 0);
}


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
