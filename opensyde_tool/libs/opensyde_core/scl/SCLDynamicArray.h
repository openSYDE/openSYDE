//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Container class for arrays of elements

   \class       stw_scl::SCLDynamicArray
   \brief       Container class for arrays of elements
   
   Provides a container class for arrays of elements of the same type.
   Original intention was to provide a target-indepentend replacement for VCL's "DynamicArray".

   Technically the class is a wrapper around std::vector.

   \copyright   Copyright 2007 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef SCLDYNAMICARRAYH
#define SCLDYNAMICARRAYH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_scl
{
//maybe this will be a part of a Borland library:
#ifndef SCL_PACKAGE
#ifdef __BORLANDC__
#define SCL_PACKAGE __declspec(package)
#else
#define SCL_PACKAGE
#endif
#endif

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//suppress PC-Lint false positives (those do have side-effects):
//lint -estring(1960,"stw_scl::SCLDynamicArray<<1>>::Delete(long)")
//lint -estring(1960,"stw_scl::SCLDynamicArray<<1>>::IncLength(long)")
//lint -estring(1960,"stw_scl::SCLDynamicArray<<1>>::Insert(long, const <1>&)")
//lint -estring(1960,"stw_scl::SCLDynamicArray<<1>>::SetLength(long)")
///Template-based array with dynamic size
template <class T> class SCL_PACKAGE SCLDynamicArray
{
private:
   std::vector<T> mc_Array; ///< actual container wrapped by SCLDynamicArray

public:
   SCLDynamicArray(void);
   SCLDynamicArray(const SCLDynamicArray<T> & orc_Src);

   virtual ~SCLDynamicArray(void);

   //Note: put the const function first here as a workaround for problems in PC-Lint 9.00k in combination with -ctc
   const T & operator [](stw_types::sint32 os32_Index) const;
   T & operator [](stw_types::sint32 os32_Index);

   SCLDynamicArray & operator =(const SCLDynamicArray<T> & orc_Src);

   void Delete(const stw_types::sint32 os32_Index);
   void Insert(const stw_types::sint32 os32_Index, const T & orc_Src);

   stw_types::sint32 GetLength(void) const;
   stw_types::sint32 GetHigh(void) const;

   void SetLength(const stw_types::sint32 os32_Length);
   void IncLength(const stw_types::sint32 os32_By = 1);
};

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set length of array

   Sets the number of elements in the array.
   New elements will be constructed using their default constructor.
   For plain numeric types this means the content will be random.

   \param[in]     os32_Length    new array length
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> void SCLDynamicArray<T>::SetLength(const stw_types::sint32 os32_Length)
{
   //do not set if it's the same Length again
   if (os32_Length != static_cast<stw_types::sint32>(mc_Array.size()))
   {
      mc_Array.resize(os32_Length);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Increase length of array

   Increase length of array.
   New elements will be constructed using their default constructor.
   For plain numeric types this means the content will be random.

   \param[in]     os32_By   number of elements to add (use a negative value to reduce length)
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> void SCLDynamicArray<T>::IncLength(const stw_types::sint32 os32_By)
{
   this->SetLength(static_cast<stw_types::sint32>(mc_Array.size()) + os32_By);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get number of entries in array

   Return length of array.

   \return
   number of elements in array
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> stw_types::sint32 SCLDynamicArray<T>::GetLength(void) const
{
   return static_cast<stw_types::sint32>(mc_Array.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get index of last entry in array

   Return highest valid index of array.
   In most cases this is one less than the length.
   If the length is zero this function will return 0.

   \return
   Index of last entry in array
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> stw_types::sint32 SCLDynamicArray<T>::GetHigh(void) const
{
   stw_types::sint32 s32_Return;
   s32_Return = static_cast<stw_types::sint32>(mc_Array.size()) - 1;
   if (s32_Return < 0)
   {
      s32_Return = 0;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get reference to array element

   Return reference to array element.
   Accessing an index that is out if range will result in undefined behavior.

   \param[in]   os32_Index  index of element to return (0 = first element)

   \return
   Reference to selected index
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> T & SCLDynamicArray<T>::operator [](const stw_types::sint32 os32_Index)
{
   return mc_Array[os32_Index];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   get const reference to array element

   Return const reference to array element.
   Accessing an index that is out if range will result in undefined behavior.

   \param[in]   os32_Index  index of element to return (0 = first element)

   \return
   Reference to selected index
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> const T & SCLDynamicArray<T>::operator [](const stw_types::sint32 os32_Index) const
{
   return mc_Array.operator [](os32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   copy constructor

   Assign content from pre-existing array.

   \param[in]   orc_Src   source array
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> SCLDynamicArray<T>::SCLDynamicArray(const SCLDynamicArray<T> & orc_Src)
{
   this->operator =(orc_Src);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default constructor

   Nothing special to do here ...
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> SCLDynamicArray<T>::SCLDynamicArray(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Nothing special to do here ...
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> SCLDynamicArray<T>::~SCLDynamicArray(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   assignment operator

   Copy specified data over existing data.

   \param[in]  orc_Src   source data to copy over

   \return  reference to (*this)
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> SCLDynamicArray<T> & SCLDynamicArray<T>::operator =(const SCLDynamicArray<T> & orc_Src)
{
   if (this != &orc_Src)
   {
      mc_Array = orc_Src.mc_Array;
   }
   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   delete one element from array

   Delete (destroy) one element from the array.
   Specifying an index that is out if range will result in undefined behavior.

   As the whole array content needs to be rearranged in memory this operation is slow.
   The content of all elements needs to be copied over.

   \param[in]  os32_Index   index of element to erase (0 = first element)
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> void SCLDynamicArray<T>::Delete(const stw_types::sint32 os32_Index)
{
   if (os32_Index > this->GetHigh())
   {
      throw "SCLDynamicArray::Delete at Invalid Position !";
   }
   typename std::vector<T>::iterator c_Index = mc_Array.begin();
   c_Index += os32_Index;
   (void)mc_Array.erase(c_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   insert one element into array

   Insert one element into the array.
   Specifying an index that is out if range will result in undefined behavior.

   As the whole array content needs to be rearranged in memory this operation is slow.
   The content of all elements needs to be copied over.

   \param[in]  os32_Index   index to insert the new element at (0 = first element)
   \param[in]  orc_Src      element to insert
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1960}  false positive in PC-Lint: as an inline function implementation this can be reused
template <class T> void SCLDynamicArray<T>::Insert(const stw_types::sint32 os32_Index, const T & orc_Src)
{
   if (os32_Index > this->GetLength()) //allow appending ...
   {
      throw "SCLDynamicArray::Insert at Invalid Position !";
   }
   typename std::vector<T>::iterator c_Index = mc_Array.begin();
   c_Index += os32_Index;
   (void)mc_Array.insert(c_Index, orc_Src);
}

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
