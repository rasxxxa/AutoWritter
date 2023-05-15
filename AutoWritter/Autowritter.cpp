#include <vector>
#include "AutoWritter.h"

static std::vector<std::string> test_sentences =
{
	"Zdravo",
	"Kako si!",
	"Sta se radi?",
	"Sta ima novo?",
	"Ovo je test"
};


void AutoWritter::ClickListener()
{
	while (true)
	{
		if ((GetKeyState(VK_LBUTTON) & 0x800) != 0)
		{
			LPPOINT point = new tagPOINT();
			if (!m_positionField.has_value())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				m_clicksField++;
				GetCursorPos(point);
				std::cout << 3 - m_clicksField << " clicks till fixation of field!" << std::endl;
				std::cout << point->x << " " << point->y << std::endl;
				if (m_clicksField >= 3)
				{
					std::lock_guard<std::recursive_mutex> lockWrite(m_writeLock);
					m_positionField.emplace(point->x, point->y);
				}
			}
			else if (!m_positionSend.has_value())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				m_clicksSendButton++;
				GetCursorPos(point);
				std::cout << 3 - m_clicksSendButton << " clicks till fixation of send button!" << std::endl;
				std::cout << point->x << " " << point->y << std::endl;
				if (m_clicksSendButton >= 3)
				{
					std::lock_guard<std::recursive_mutex> lockWrite(m_writeLock);
					m_positionSend.emplace(point->x, point->y);
					m_conditional.notify_all();
				}
			}
			delete point;
		}
		else if ((GetKeyState(VK_RBUTTON) & 0x800) != 0)
		{
			std::lock_guard<std::recursive_mutex> lockWrite(m_writeLock);
			m_clicksField = 0;
			m_clicksSendButton = 0;
			m_positionField.reset();
			m_positionSend.reset();
		}
	}
}

void AutoWritter::RunAutoClickThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_conditionalLock);
		m_conditional.wait(lock, [this] { return m_positionSend.has_value() && test_sentences.size() > 0; });
		
		for (unsigned sent = 0; sent < test_sentences.size() && m_positionSend.has_value(); sent++)
		{
			std::unique_lock<std::recursive_mutex> lockWrite(m_writeLock);
			SetCursorPos(m_positionField.value().first, m_positionField.value().second);
			for (char ch : test_sentences[sent])
			{
				int upperCode = std::toupper(ch);
				INPUT inputPress;
				inputPress.type = INPUT_KEYBOARD;
				inputPress.ki.wVk = upperCode;
				inputPress.ki.dwFlags = 0;

				INPUT inputRelease;
				inputRelease.type = INPUT_KEYBOARD;
				inputRelease.ki.wVk = upperCode;
				inputRelease.ki.dwFlags = KEYEVENTF_KEYUP;

				INPUT inputEvents[2] = { inputPress, inputRelease };
				SendInput(2, inputEvents, sizeof(INPUT));

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			SetCursorPos(m_positionSend.value().first, m_positionSend.value().second);
			mouse_event(MOUSEEVENTF_LEFTDOWN, m_positionSend.value().first, m_positionSend.value().second, 0, 0);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			mouse_event(MOUSEEVENTF_LEFTUP, m_positionSend.value().first, m_positionSend.value().second, 0, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		test_sentences.clear();
	}
}

void AutoWritter::RunAutoclicker()
{
	m_clickListener = std::jthread(&AutoWritter::ClickListener, this);
	m_clicker = std::jthread(&AutoWritter::RunAutoClickThread, this);
	m_clicker.join();
}
