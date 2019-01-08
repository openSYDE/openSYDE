//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box for param tree (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECBXPARAM_H
#define C_OGECBXPARAM_H

/* -- Includes ------------------------------------------------------------- */
#include <QComboBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeCbxParam :
   public QComboBox
{
   Q_OBJECT

public:
   explicit C_OgeCbxParam(QWidget * const opc_Parent = NULL);
   ~C_OgeCbxParam(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_OgeCbxParam(const C_OgeCbxParam &);
   C_OgeCbxParam & operator =(const C_OgeCbxParam &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
