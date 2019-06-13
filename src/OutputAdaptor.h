#ifndef OUTPUT_ADAPTOR_H
#define OUTPUT_ADAPTOR_H

#include <exception>

/**
* @brief Interface for an output adaptor.
* 
* Output adaptors deliver finalised buffers of signed, 16-bit samples
* to the audio system using a push model, blocking until the audio backend
* needs more samples.
*/
class OutputAdaptor {

public:	
	/**
	 * Write some samples to the output.
	 * 
	 * Send a buffer of signed 16-bit samples to the audio system.
	 * The buffer size must be the same as the adaptors internal
	 * buffer, which can be discovered using its size() method.
	 * 
	 * This is a convenience method to provide a transparent way
	 * of converting the sample data to the underlying type the
	 * audio system wants.
	 * 
	 * @param buffer Buffer containing 16-bit signed sample data
	 */
	virtual void writeSamples(short buffer[]) = 0;
	 
	/**
	 * Some OutputAdaptors need to set the sample rate and buffer size
	 * themselves, overriding the SMRGyGurdy configured values.
	 * Query the required sample rate, or 0 if there is no such
	 * requirement.
	 * 
	 * @return the sample rate required by the audio system
	 */
	virtual const unsigned int rate(void) const { return 0; }

	/**
	 * Some OutputAdaptors need to set the sample rate and buffer size
	 * themselves, overriding the SMRGyGurdy configured values.
	 * Query the required buffer size, or 0 if there is no such
	 * requirement.
	 * 
	 * @return the buffer size in frames required by the audio system
	 */
	virtual const size_t size(void) const { return 0; }
	
	/**
	 * Start the backend. Where the backend supports a push protocol,
	 * this does nothing. If it's a pull protocol, it may be called
	 * after all the other initialisation has happened to prevent
	 * a delay causing overruns.
	 */
	virtual void commence(void) { }

	/* Exception List */

	/** Thrown when pcm could not be opened */
	class OutputAdaptorException : public std::exception {
		
		const char* reason;
	public:
		OutputAdaptorException(
			const char* reason = "Audio output adaptor exception"
		) : reason(reason) { }
		
		virtual const char* what(void) throw() {
			return reason;
		}
	};

};

#endif // OUTPUT_ADAPTOR_H
