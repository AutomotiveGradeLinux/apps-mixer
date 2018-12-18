#include "audiorole.hpp"

AudioRole::AudioRole(QObject* parent)
	: QObject(parent)
	, m_Name{""}
	, m_Value{0}
	, m_Updating{0}
{
}

AudioRole::AudioRole(const QString& name, int value, QObject* parent)
	: QObject(parent)
	, m_Name{name}
	, m_Value{value}
	, m_Updating{0}
{
}

QString AudioRole::Name() const
{
	return m_Name;
}

void AudioRole::setName(const QString& name)
{
	m_Name = name;
	emit NameChanged();
}

int AudioRole::Value() const
{
	return m_Value;
}

void AudioRole::setValue(int value)
{
	if (m_Value != value)
	{
		m_Value = value;
		if (m_Updating == 0)
			emit ValueChanged();
	}
}

void AudioRole::BeginUpdate()
{
	m_Updating++;
}

void AudioRole::EndUpdate()
{
	if (m_Updating > 0) m_Updating--;
	//if (m_Updating == 0) emit ValueChanged();
}
