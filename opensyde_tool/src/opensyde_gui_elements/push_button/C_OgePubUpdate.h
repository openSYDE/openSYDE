//-----------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBUPDATE_H
#define C_OGEPUBUPDATE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubStandard.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubUpdate :
   public C_OgePubStandard
{
   Q_OBJECT

public:
   C_OgePubUpdate(QWidget * const opc_Parent = NULL);
   void InitCustomIcon(QString oc_IconPathEnabled, QString oc_IconPathDisabled);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   QIcon mc_IconEnabled;
   QIcon mc_IconDisabled;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
