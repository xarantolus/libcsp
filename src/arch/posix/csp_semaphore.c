#include "../../csp_semaphore.h"

#include <inttypes.h>
#include <csp/csp.h>
#include <csp/csp_debug.h>

#include <time.h>

#if defined(__APPLE__)
/* macOS: pthread-based binary semaphore (see csp_semaphore.h). Semantics
 * mirror the sem_t version below, including the binary clamp in
 * csp_bin_sem_post (posting an unlocked semaphore is a no-op). */

void csp_bin_sem_init(csp_bin_sem_t * sem) {

	pthread_mutex_init(&sem->mutex, NULL);
	pthread_cond_init(&sem->cond, NULL);
	sem->value = 1;
}

int csp_bin_sem_wait(csp_bin_sem_t * sem, unsigned int timeout) {

	int ret = 0;

	pthread_mutex_lock(&sem->mutex);
	if (timeout == CSP_MAX_TIMEOUT) {
		while (sem->value == 0 && ret == 0) {
			ret = pthread_cond_wait(&sem->cond, &sem->mutex);
		}
	} else {
		struct timespec ts;
		if (clock_gettime(CLOCK_REALTIME, &ts)) {
			pthread_mutex_unlock(&sem->mutex);
			return CSP_SEMAPHORE_ERROR;
		}
		ts.tv_sec += timeout / 1000;
		ts.tv_nsec += (long)(timeout % 1000) * 1000000L;
		if (ts.tv_nsec >= 1000000000L) {
			ts.tv_sec++;
			ts.tv_nsec -= 1000000000L;
		}
		while (sem->value == 0 && ret == 0) {
			ret = pthread_cond_timedwait(&sem->cond, &sem->mutex, &ts);
		}
	}
	if (ret == 0) {
		sem->value = 0;
	}
	pthread_mutex_unlock(&sem->mutex);

	return (ret == 0) ? CSP_SEMAPHORE_OK : CSP_SEMAPHORE_ERROR;
}

int csp_bin_sem_post(csp_bin_sem_t * sem) {

	pthread_mutex_lock(&sem->mutex);
	if (sem->value == 0) {
		sem->value = 1;
		pthread_cond_signal(&sem->cond);
	}
	pthread_mutex_unlock(&sem->mutex);

	return CSP_SEMAPHORE_OK;
}

#else /* !__APPLE__ */

#include <semaphore.h>

void csp_bin_sem_init(csp_bin_sem_t * sem) {

	sem_init((sem_t *) sem, 0, 1);
}

int csp_bin_sem_wait(csp_bin_sem_t * sem, unsigned int timeout) {

	int ret;

	if (timeout == CSP_MAX_TIMEOUT) {
		ret = sem_wait((sem_t *) sem);
	} else {
		struct timespec ts;
		if (clock_gettime(CLOCK_REALTIME, &ts)) {
			return CSP_SEMAPHORE_ERROR;
		}

		uint32_t sec = timeout / 1000;
		uint32_t nsec = (timeout - 1000 * sec) * 1000000;

		ts.tv_sec += sec;

		if (ts.tv_nsec + nsec >= 1000000000) {
			ts.tv_sec++;
		}

		ts.tv_nsec = (ts.tv_nsec + nsec) % 1000000000;

		ret = sem_timedwait((sem_t *) sem, &ts);
	}

	if (ret != 0)
		return CSP_SEMAPHORE_ERROR;

	return CSP_SEMAPHORE_OK;
}

int csp_bin_sem_post(csp_bin_sem_t * sem) {

	int value;
	sem_getvalue((sem_t *) sem, &value);
	if (value > 0) {
		return CSP_SEMAPHORE_OK;
	}

	if (sem_post((sem_t *) sem) == 0) {
		return CSP_SEMAPHORE_OK;
	}

	return CSP_SEMAPHORE_ERROR;
}

#endif /* !__APPLE__ */
