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

#include <QGuiApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QUrlQuery>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QDebug>

#include "mixer.hpp"
#include "audiorole.hpp"

int main(int argc, char *argv[])
{
        QGuiApplication app(argc, argv);
        app.setDesktopFileName("mixer");

	QQmlApplicationEngine engine;
	QQmlContext *context = engine.rootContext();

	QCommandLineParser parser;
	parser.addPositionalArgument("port", app.translate("main", "port for binding"));
	parser.addPositionalArgument("secret", app.translate("main", "secret for binding"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);
	QStringList positionalArguments = parser.positionalArguments();

	if (positionalArguments.length() == 2) {
		int port = positionalArguments.takeFirst().toInt();
		QString secret = positionalArguments.takeFirst();

		QUrl bindingAddress;
		QUrlQuery query;

		bindingAddress.setScheme(QStringLiteral("ws"));
		bindingAddress.setHost(QStringLiteral("localhost"));
		bindingAddress.setPort(port);
		bindingAddress.setPath(QStringLiteral("/api"));


		query.addQueryItem(QStringLiteral("token"), secret);
		bindingAddress.setQuery(query);
		context->setContextProperty(QStringLiteral("bindingAddress"), bindingAddress);

		qmlRegisterType<Mixer>("Mixer", 1, 0, "Mixer");
	}

	engine.load(QUrl(QStringLiteral("qrc:/Mixer.qml")));
        return app.exec();

}
