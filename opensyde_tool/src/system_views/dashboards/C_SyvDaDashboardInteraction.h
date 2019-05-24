//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for dashboard interactions (connect and dark mode)
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDINTERACTION_H
#define C_SYVDADASHBOARDINTERACTION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMovie>
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaDashboardInteraction;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardInteraction :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaDashboardInteraction(QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardInteraction(void);

   void InitStaticNames(void) const;

   void SetPushButtonConnectSvg(const QString & orc_IconPath, const bool oq_ShowAnimation);
   void SetPushButtonDarkSvg(const QString & orc_IconPath) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigPushButtonDarkModePressed(void);
   void SigPushButtonConnectPressed(void);

private:
   QMovie * const mpc_Movie;
   Ui::C_SyvDaDashboardInteraction * mpc_Ui;

   void m_RestartMovie(void);

   //Avoid call
   C_SyvDaDashboardInteraction(const C_SyvDaDashboardInteraction &);
   C_SyvDaDashboardInteraction & operator =(const C_SyvDaDashboardInteraction &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
