#include"CpuClass.h"

CpuClass::CpuClass() {

}

CpuClass::~CpuClass() {

}

void CpuClass::Initialize() {
	PDH_STATUS status;

	//初始化指示该对象是否可以读取系统CPU使用率的标志
	m_canReadCpu = true;

	//创建查询对象来轮询CPU使用情况
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS) {
		m_canReadCpu = false;
	}

	//设置查询对象以轮询系统中的所有CPU
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS) {
		m_canReadCpu = false;
	}

	m_lastSampleTime = GetTickCount();
	
	m_cpuUsage = 0;

	return;
}

void CpuClass::Shutdown() {
	if (m_canReadCpu) {
		PdhCloseQuery(m_queryHandle);
	}

	return;
}

void CpuClass::Frame() {
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu) {
		if ((m_lastSampleTime + 1000) < GetTickCount()) {
			m_lastSampleTime = GetTickCount();
			PdhCollectQueryData(m_queryHandle);
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);
			m_cpuUsage = value.largeValue;
		}
	}

	return;
}

int CpuClass::GetCpuPercentage() {
	int usage;
	
	if (m_canReadCpu) {
		usage = (int)m_cpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}