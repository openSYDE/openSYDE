//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Logging data structure (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMPROLOGGINGDATA_H
#define C_CAMPROLOGGINGDATA_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamProLoggingData
{
public:
   enum E_OverwriteMode
   {
      eADD_TIMESTAMP = 0,
      eWARN_BEFORE_OVERWRITE,
      eOVERWRITE
   };

   enum E_Format
   {
      eASC = 0,
      eBLF
   };

   C_CamProLoggingData(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   void Clear(void);

   static QString h_FormatEnumToString(const E_Format oe_Format);
   static E_Format h_FormatStringToEnum(const QString & orc_Format);

   QString c_Directory;
   QString c_FileName;
   bool q_Enabled;
   E_OverwriteMode e_OverwriteMode;
   E_Format e_FileFormat;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
