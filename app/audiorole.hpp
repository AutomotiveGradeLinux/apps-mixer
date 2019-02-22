#ifndef AUDIOROLE_H
#define AUDIOROLE_H

#include <QObject>

class AudioRole
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ Name WRITE setName NOTIFY NameChanged)
	Q_PROPERTY(int value READ Value WRITE setValue NOTIFY ValueChanged)

private:
	QString m_Name;
	int m_Value;
	int m_Updating;

public:
	explicit AudioRole(QObject* parent = nullptr);
	explicit AudioRole(const QString& name, int value, QObject* parent = nullptr);

	QString Name() const;
	void setName(const QString& name);

	int Value() const;
	void setValue(int value);

	void BeginUpdate();
	bool EndUpdate();

signals:
	void NameChanged();
	void ValueChanged();

public slots:
};

#endif // AUDIOROLE_H
