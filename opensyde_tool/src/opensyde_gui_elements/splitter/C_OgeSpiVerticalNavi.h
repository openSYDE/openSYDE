//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Vertical splitter for navi bar (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPIVERTICALNAVI_H
#define C_OGESPIVERTICALNAVI_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_OgeSpiBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpiVerticalNavi :
   public C_OgeSpiBase
{
   Q_OBJECT

public:
   C_OgeSpiVerticalNavi(QWidget * const opc_Parent = NULL);

   void LoadUserSettings(void);
   void StoreUserSettings(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QSplitterHandle * createHandle(void) override;
   //lint -restore

private:
   bool mq_Loaded;
   stw_types::sintn msn_LastNaviBarSize;

   void m_OnDoubleClick(void);
};

class C_OgeSpiVerticalNaviHandle :
   public QSplitterHandle
{
   Q_OBJECT

public:
   C_OgeSpiVerticalNaviHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDoubleClick(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
