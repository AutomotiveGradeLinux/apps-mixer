/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2016,2017 Konsulko Group
 * Copyright (C) 2018 IoT.bzh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QtCore/QDebug>
#include <QtCore/QCommandLineParser>
#include <QtCore/QUrlQuery>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QStandardPaths>
#include <QtCore/QThread>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuickControls2/QQuickStyle>
#include <QtQml/qqml.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickview.h>
#include <QQuickWindow>
#ifndef NATIVE_BUILD
#include <libhomescreen.hpp>
#include <qlibwindowmanager.h>
#else
#include <QScreen>
#endif
#include "mixer.hpp"
#include "audiorole.hpp"

int main(int argc, char *argv[])
{
	QString graphic_role = QString("mixer");

	QGuiApplication app(argc, argv);

	QQuickStyle::setStyle("AGL");

	QCommandLineParser parser;
	parser.addPositionalArgument("port", app.translate("main", "port for binding"));
	parser.addPositionalArgument("secret", app.translate("main", "secret for binding"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);
	QStringList positionalArguments = parser.positionalArguments();

	qmlRegisterType<Mixer>("Mixer", 1, 0, "Mixer");

	QQmlApplicationEngine engine;
	if (positionalArguments.length() == 2) {
		int port = positionalArguments.takeFirst().toInt();
		QString secret = positionalArguments.takeFirst();
		QUrl bindingAddress;
		bindingAddress.setScheme(QStringLiteral("ws"));
		bindingAddress.setHost(QStringLiteral("localhost"));
		bindingAddress.setPort(port);
		bindingAddress.setPath(QStringLiteral("/api"));
		QUrlQuery query;
		query.addQueryItem(QStringLiteral("token"), secret);
		bindingAddress.setQuery(query);
		QQmlContext *context = engine.rootContext();
		context->setContextProperty(QStringLiteral("bindingAddress"), bindingAddress);
		qDebug() << "Connect to: " << bindingAddress;


#ifndef NATIVE_BUILD
		LibHomeScreen* hs = new LibHomeScreen();
		QLibWindowmanager* qwm = new QLibWindowmanager();

		// WindowManager
		if(qwm->init(port,secret) != 0){
			exit(EXIT_FAILURE);
		}
		AGLScreenInfo screenInfo(qwm->get_scale_factor());
		engine.rootContext()->setContextProperty(QStringLiteral("scale_factor"), screenInfo.scale_factor());
		// Request a surface as described in layers.json windowmanager’s file
		if (qwm->requestSurface(graphic_role) != 0) {
			exit(EXIT_FAILURE);
		}
		// Create an event callbnewack against an event type. Here a lambda is called when SyncDraw event occurs
		qwm->set_event_handler(QLibWindowmanager::Event_SyncDraw, [qwm, &graphic_role](json_object*) {
			fprintf(stderr, "Surface got syncDraw!\n");
			qwm->endDraw(graphic_role);
		});

		// HomeScreen
		std::string token = secret.toStdString();
		hs->init(port, token.c_str());
		// Set the event handler for Event_TapShortcut which will activate the surface for windowmanager
		hs->set_event_handler(LibHomeScreen::Event_TapShortcut, [qwm, &graphic_role](json_object *object){
			qDebug("Surface %s got tapShortcut\n", graphic_role.toStdString().c_str());
			qwm->activateWindow(graphic_role);
		});
#else
		engine.rootContext()->setContextProperty(QStringLiteral("scale_factor"), 1.0);
#endif
		engine.load(QUrl(QStringLiteral("qrc:/Mixer.qml")));

		// Find the instantiated model QObject and connect the signals/slots
		QList<QObject*> mobjs = engine.rootObjects();
		if (mobjs.empty())
		{
			qDebug() << "[ERROR] Failed to load QML!";
			return -1;
		}

		QQuickWindow *window = qobject_cast<QQuickWindow *>(mobjs.first());
#ifdef NATIVE_BUILD
		window->setFlags(window->flags() & ~Qt::FramelessWindowHint); // Remove the borderless flag
		window->setHeight(QGuiApplication::primaryScreen()->geometry().height());
#else
		QObject::connect(window, SIGNAL(frameSwapped()), qwm, SLOT(slotActivateWindow()));
#endif
	}
	else
	{
		qDebug() << "[ERROR] No port and token specified!";
		return -1;
	}

	return app.exec();
}
