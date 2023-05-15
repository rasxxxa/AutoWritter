#pragma once
#include <atomic>
#include <condition_variable>
#include <Windows.h>
#include <optional>
#include <iostream>
#include <barrier>
#include <thread>

struct AutoWritterSetup
{
	unsigned sleepBetweenTypes;
};

class AutoWritter final
{
private:
	std::atomic_int m_clicksField, m_clicksSendButton;
	std::condition_variable m_conditional;
	std::recursive_mutex m_writeLock;
	std::mutex m_conditionalLock;
	std::jthread m_clickListener, m_clicker;
	unsigned m_sleepBetweenClicks;
	std::optional<std::pair<unsigned, unsigned>> m_positionField, m_positionSend;
	void ClickListener();
	void RunAutoClickThread();
public:
	AutoWritter(const AutoWritterSetup& setup) : m_clicksField(0), m_clicksSendButton(0), m_sleepBetweenClicks(setup.sleepBetweenTypes)
	{

	}

	AutoWritter(const AutoWritter&) = delete;
	AutoWritter() = default;
	AutoWritter& operator=(const AutoWritter&) = delete;

	void RunAutoclicker();
};

