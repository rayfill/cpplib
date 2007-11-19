#ifndef EXIF_HPP_
#define EXIF_HPP_

#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <ostream>
#include <IO/Endian.hpp>
#include <math/Rational.hpp>


namespace Exif
{
	/**
	 * タグ情報
	 */
	class TagInfo
	{
	private:
		unsigned short tag;
		unsigned short tagType;
		unsigned int numberOfValues;
		unsigned int valueOrOffset;

	public:
		static TagInfo
		parseMemoryToTagInfo(const void* pointer,
							 const UserDefinedEndianConverter& conv)
		{
			return TagInfo(
				conv.from(
					*reinterpret_cast<const unsigned short*>(pointer)),
				conv.from(
					*(reinterpret_cast<const unsigned short*>(pointer) + 1)),
				conv.from(
					*(reinterpret_cast<const unsigned int*>(pointer) + 1)),
				conv.from(
					*(reinterpret_cast<const unsigned int*>(pointer) + 2)));
		}

		unsigned short getTag() const
		{
			return tag;
		}

		unsigned short getTagType() const
		{
			return tagType;
		}

		unsigned int getNumberOfValues() const
		{
			return numberOfValues;
		}

		unsigned int getValueOrOffset() const
		{
			return valueOrOffset;
		}

		TagInfo(const unsigned short tag_ = 0,
				const unsigned short tagType_ = 0,
				const unsigned int numberOfValues_ = 0,
				const unsigned int valueOrOffset_ = 0)
			: tag(tag_),
			  tagType(tagType_),
			  numberOfValues(numberOfValues_),
			  valueOrOffset(valueOrOffset_)
		{}

		~TagInfo()
		{}

	};

	/**
	 * タグ情報解析器
	 */
	class TagInfoParser
	{
		friend class ExifTest;

	private:
		TagInfoParser(const TagInfoParser&);
		TagInfoParser& operator=(const TagInfoParser&);

		const void* resourceHead;
		const void* resourceTail;
		const UserDefinedEndianConverter& converter;

		bool rangeCheck(const TagInfo& tag) const
		{
			const void* pointer = getResourceOffset(tag);
			const size_t length = tag.getNumberOfValues();
			const size_t typeSize = getValueSize(tag);

			if (pointer >= resourceHead &&
				reinterpret_cast<const char*>(pointer) +
				(length * typeSize) <= resourceTail)
				return true;
			return false;
		}

		size_t getValueSize(
			const TagInfo& tag) const
		{
			switch(tag.getTagType())
			{
			case 1: // unsigned char
			case 2: // asciz string
			case 7: // undefined byte stream
				return 1;
				break;

			case 3: // 16bit unsigned short
				return 2;
				break;

			case 4: // 32bit unsigned int
			case 9: // 32bit signed int
				return 4;
				break;

			case 5: // unsigned rational(32bit unsigned int * 2)
			case 10: // signed rational
				return 8;
				break;

			default:
				throw std::runtime_error("argument is invalid tag type.");
			}
		}

		bool isResourceReference(
			const TagInfo& tag) const
		{
			if (getValueSize(tag) * tag.getNumberOfValues() >
				sizeof(unsigned int))
				return true;
			return false;
		}

		const void* getResourceOffset(
			const TagInfo& tag) const
		{
			return
				reinterpret_cast<const unsigned char*>(resourceHead) +
				tag.getValueOrOffset();
		}

		template <typename TargetType>
		struct ConstructorTraits
		{
			const UserDefinedEndianConverter& converter;
			ConstructorTraits(const UserDefinedEndianConverter& converter_)
				: converter(converter_)
			{}
			
			TargetType operator()(const void* pointer, const size_t offsetCount)
			{
				return converter.from(
					TargetType(
						*(reinterpret_cast<const TargetType*>
						  (pointer) + offsetCount)));
			}
		};

		template <typename RationalBaseType>
		struct RationalConstructorTraits
		{
			const UserDefinedEndianConverter& converter;
			RationalConstructorTraits(
				const UserDefinedEndianConverter& converter_)
				: converter(converter_)
			{}

			Rational<RationalBaseType>
			operator()(const void* pointer, const size_t offsetCount)
			{
				return Rational<RationalBaseType>(
					converter.from(
						*(reinterpret_cast<const RationalBaseType*>(
							  pointer) + offsetCount * 2)),
					converter.from(
						*(reinterpret_cast<const RationalBaseType*>(
							  pointer) + offsetCount * 2 + 1)));
			}
		};

		template<typename convType,
			typename CtorTraits>
		std::vector<convType> getVectorFromTag(const TagInfo& tag)
			const 
 		{
			CtorTraits traits = CtorTraits(converter);
			std::vector<convType> result;
			if (isResourceReference(tag))
			{
				// read from resource
				if (!rangeCheck(tag))
					throw std::runtime_error("invalid tag data.");

				for (unsigned int count = 0;
					 count < tag.getNumberOfValues();
					 ++count)
					result.push_back(
						traits(
							getResourceOffset(tag),
							count));
			}
			else
			{
				// read from tag
				const unsigned int beVal =
					converter.to(tag.getValueOrOffset());

				for (unsigned int count = 0;
					 count < tag.getNumberOfValues();
					 ++count)
					result.push_back(
						traits(
							&beVal, count));
			}
			return result;
		}
	public:	
		std::string getString(const TagInfo& tag)
			const 
		{
			std::vector<char> stringSource =
				getVectorFromTag<char, ConstructorTraits<char> >(tag);

			if(stringSource.back() != 0)
				throw std::runtime_error(
					"invalid tag data, for not null terminated string.");

			return std::string(&stringSource[0]);
		}

		std::vector<unsigned char> getBytes(const TagInfo& tag)
			const 
		{
			return
				getVectorFromTag<unsigned char,
				ConstructorTraits<unsigned char> >(tag);
		}

		std::vector<unsigned short> getUnsignedShorts(const TagInfo& tag)
			const 
		{
			return
				getVectorFromTag<unsigned short,
				ConstructorTraits<unsigned short> >(tag);
		}

		std::vector<unsigned int> getUnsignedLongs(const TagInfo& tag)
			const 
		{
			return getVectorFromTag<unsigned int,
				ConstructorTraits<unsigned int> >(tag);
		}

		std::vector<int> getLongs(const TagInfo& tag)
			const 
		{
			return getVectorFromTag<int,
				ConstructorTraits<int> >(tag);
		}

		std::vector<Rational<unsigned int> >
		getUnsignedRationals(const TagInfo& tag)
			const 
		{
			return getVectorFromTag<Rational<unsigned int>,
				RationalConstructorTraits<unsigned int> >(tag);
		}

		std::vector<Rational<int> > getRationals(const TagInfo& tag)
			const 
		{
			return getVectorFromTag<Rational<int>,
				RationalConstructorTraits<int> >(tag);
		}

		TagInfoParser(const void* head,
					  const void* tail,
					  const UserDefinedEndianConverter& converter_)
			: resourceHead(head),
			  resourceTail(tail),
			  converter(converter_)
		{}

		~TagInfoParser() 
		{}
	};

	/**
	 * Exif情報構造体
	 */
	struct ExifInfo
	{
		// for image information
		unsigned int imageWidth;
		unsigned int imageHeight;
		unsigned short bitsPerSample[3];
		unsigned short compression;
		unsigned short photometricInterpretation;
		unsigned short orientation;
		unsigned short samplesPerPixel;
		unsigned short planarConfiguration;
		unsigned short YCbCrSubSampling[2];
		unsigned short YCbCrPositioning;
		unsigned short resolutionUnit;
		Rational<unsigned int> xResolution;
		Rational<unsigned int> yResolution;

		// for image thumbnail data posision information.
		std::vector<unsigned int> stripOffsets;
		unsigned int rowsPerStrip;
		std::vector<unsigned int> stripByteCounts;
		unsigned int JPEGInterchangeFormat;
		unsigned int JPEGInterchangeFormatLength;

		// image data traits.
		std::vector<unsigned short> transferFunction;
		Rational<unsigned int> whitePoint[2];
		Rational<unsigned int> primaryChromaticities[6];
		Rational<unsigned int> YCbCrCoefficients[3];
		Rational<unsigned int> referenceBlackWhite[6];

		// other.
		std::string dateTime;
		std::string imageDescription;
		std::string make;
		std::string model;
		std::string software;
		std::string artist;
		std::string copyright;

		struct ExifDirectory
		{
			unsigned char exifVersion[4];
			unsigned char flashPixVersion[4];
			unsigned short colorSpace;

			unsigned char componentsConfiguration[4];
			Rational<unsigned int> compressedBitsPerPixel;
			unsigned int pixelXDimension;
			unsigned int pixelYDimension;

			std::vector<unsigned char> makerNote;
			std::vector<unsigned char> userComment;
			std::string relatedSoundFile;
			std::string dateTimeOriginal;
			std::string dateTimeDigitaized;
			std::string subsecTime;
			std::string subsecTimeOriginal;
			std::string subsecTimeDigitaized;

			// photo information
			Rational<unsigned int> exposureTime;
			Rational<unsigned int> fNumber;
			unsigned short exposureProgram;
			std::string spectralSensitivity;
			std::vector<unsigned short> isoSpeedRatings;
			// Optical to Electron Conversion Function.
			std::vector<unsigned char> OECF;
			Rational<int> shutterSpeedValue;
			Rational<unsigned int> apertureValue;
			Rational<int> brightnessValue;
			Rational<int> exposureBiasValue;
			Rational<unsigned int> maxApertureValue;
			Rational<unsigned int> subjectDistance;
			unsigned short meteringMode;
			unsigned short lightSource;
			unsigned short flash;
			Rational<unsigned int> focalLength;
			Rational<unsigned int> flashEnergy;
			std::vector<unsigned char> spatialFrequencyResponse;
			Rational<unsigned int> focalPlaneXResolution;
			Rational<unsigned int> focalPlaneYResolution;
			unsigned short focalPlaneResoutionUnit;
			unsigned short subjectLocation[2];
			Rational<unsigned int> exposureIndex;
			unsigned short sensingMethod;
			unsigned char fileSource;
			unsigned char sceneType;
			std::vector<unsigned char> cfaPattern;

			unsigned int interoperabilityIFDPointer;

			ExifDirectory()
				: exifVersion(),
				  flashPixVersion(),
				  colorSpace(),
				  componentsConfiguration(),
				  compressedBitsPerPixel(),
				  pixelXDimension(),
				  pixelYDimension(),
				  makerNote(),
				  userComment(),
				  relatedSoundFile(),
				  dateTimeOriginal(),
				  dateTimeDigitaized(),
				  subsecTime(),
				  subsecTimeOriginal(),
				  subsecTimeDigitaized(),
				  exposureTime(),
				  fNumber(),
				  exposureProgram(),
				  spectralSensitivity(),
				  isoSpeedRatings(),
				  OECF(),
				  shutterSpeedValue(),
				  apertureValue(),
				  brightnessValue(),
				  exposureBiasValue(),
				  maxApertureValue(),
				  subjectDistance(),
				  meteringMode(),
				  lightSource(),
				  flash(),
				  focalLength(),
				  flashEnergy(),
				  spatialFrequencyResponse(),
				  focalPlaneXResolution(),
				  focalPlaneYResolution(),
				  focalPlaneResoutionUnit(),
				  subjectLocation(),
				  exposureIndex(),
				  sensingMethod(),
				  fileSource(),
				  sceneType(),
				  cfaPattern(),
				  interoperabilityIFDPointer()
			{}

			ExifDirectory(const ExifDirectory& source)
				: exifVersion(),
				  flashPixVersion(),
				  colorSpace(source.colorSpace),
				  componentsConfiguration(),
				  compressedBitsPerPixel(source.compressedBitsPerPixel),
				  pixelXDimension(source.pixelXDimension),
				  pixelYDimension(source.pixelYDimension),
				  makerNote(source.makerNote),
				  userComment(source.userComment),
				  relatedSoundFile(source.relatedSoundFile),
				  dateTimeOriginal(source.dateTimeOriginal),
				  dateTimeDigitaized(source.dateTimeDigitaized),
				  subsecTime(source.subsecTime),
				  subsecTimeOriginal(source.subsecTimeOriginal),
				  subsecTimeDigitaized(source.subsecTimeDigitaized),
				  exposureTime(source.exposureTime),
				  fNumber(source.fNumber),
				  exposureProgram(source.exposureProgram),
				  spectralSensitivity(source.spectralSensitivity),
				  isoSpeedRatings(source.isoSpeedRatings),
				  OECF(source.OECF),
				  shutterSpeedValue(source.shutterSpeedValue),
				  apertureValue(source.apertureValue),
				  brightnessValue(source.brightnessValue),
				  exposureBiasValue(source.exposureBiasValue),
				  maxApertureValue(source.maxApertureValue),
				  subjectDistance(source.subjectDistance),
				  meteringMode(source.meteringMode),
				  lightSource(source.lightSource),
				  flash(source.flash),
				  focalLength(source.focalLength),
				  flashEnergy(source.flashEnergy),
				  spatialFrequencyResponse(source.spatialFrequencyResponse),
				  focalPlaneXResolution(source.focalPlaneXResolution),
				  focalPlaneYResolution(source.focalPlaneYResolution),
				  focalPlaneResoutionUnit(source.focalPlaneResoutionUnit),
				  subjectLocation(),
				  exposureIndex(source.exposureIndex),
				  sensingMethod(source.sensingMethod),
				  fileSource(source.fileSource),
				  sceneType(source.sceneType),
				  cfaPattern(source.cfaPattern),
				  interoperabilityIFDPointer(source.interoperabilityIFDPointer)
			{
				std::copy(source.exifVersion,
						  source.exifVersion +
						  sizeof(source.exifVersion),
						  exifVersion);
				std::copy(source.flashPixVersion,
						  source.flashPixVersion +
						  sizeof(source.flashPixVersion),
						  flashPixVersion);
				std::copy(source.componentsConfiguration,
						  source.componentsConfiguration +
						  sizeof(source.componentsConfiguration),
						  componentsConfiguration);
				std::copy(source.subjectLocation,
						  source.subjectLocation +
						  sizeof(source.subjectLocation),
						  subjectLocation);
			}

			ExifDirectory& operator=(const ExifDirectory& source)
			{
				if (this == &source)
					return *this;

				std::copy(source.exifVersion,
						  source.exifVersion +
						  sizeof(source.exifVersion),
						  exifVersion);
				std::copy(source.flashPixVersion,
						  source.flashPixVersion +
						  sizeof(source.flashPixVersion),
						  flashPixVersion);
				colorSpace = source.colorSpace;
				std::copy(source.componentsConfiguration,
						  source.componentsConfiguration +
						  sizeof(source.componentsConfiguration),
						  componentsConfiguration);
				compressedBitsPerPixel = source.compressedBitsPerPixel;
				pixelXDimension = source.pixelXDimension;
				pixelYDimension = source.pixelYDimension;
				makerNote = source.makerNote;
				userComment = source.userComment;
				relatedSoundFile = source.relatedSoundFile;
				dateTimeOriginal = source.dateTimeOriginal;
				dateTimeDigitaized = source.dateTimeDigitaized;
				subsecTime = source.subsecTime;
				subsecTimeOriginal = source.subsecTimeOriginal;
				subsecTimeDigitaized = source.subsecTimeDigitaized;
				exposureTime = source.exposureTime;
				fNumber = source.fNumber;
				exposureProgram = source.exposureProgram;
				spectralSensitivity = source.spectralSensitivity;
				isoSpeedRatings = source.isoSpeedRatings;
				OECF = source.OECF;
				shutterSpeedValue = source.shutterSpeedValue;
				apertureValue = source.apertureValue;
				brightnessValue = source.brightnessValue;
				exposureBiasValue = source.exposureBiasValue;
				maxApertureValue = source.maxApertureValue;
				subjectDistance = source.subjectDistance;
				meteringMode = source.meteringMode;
				lightSource = source.lightSource;
				flash = source.flash;
				focalLength = source.focalLength;
				flashEnergy = source.flashEnergy;
				spatialFrequencyResponse = source.spatialFrequencyResponse;
				focalPlaneXResolution = source.focalPlaneXResolution;
				focalPlaneYResolution = source.focalPlaneYResolution;
				focalPlaneResoutionUnit = source.focalPlaneResoutionUnit;
				std::copy(source.subjectLocation,
						  source.subjectLocation +
						  sizeof(source.subjectLocation),
						  subjectLocation);
				exposureIndex = source.exposureIndex;
				sensingMethod = source.sensingMethod;
				fileSource = source.fileSource;
				sceneType = source.sceneType;
				cfaPattern = source.cfaPattern;

				return *this;
			}

			friend std::ostream& operator<<(
				std::ostream& out,
				const ExifDirectory& info)
			{
				out << "Exif version: " <<
					std::string(&info.exifVersion[0],
								&info.exifVersion[4]) << std::endl;

				out << "FlashPix version: " <<
					std::string(&info.flashPixVersion[0],
								&info.flashPixVersion[4]) << std::endl;
				
				out << "Color space: ";
				if (info.colorSpace == 1)
					out << "sRGB" << std::endl;
				else if (info.colorSpace == 0xffff)
					out << "Uncalibrated" << std::endl;
				else
					out << "reserved(unknown)." << std::endl;


				out << "Pixel x dimension: " <<
					info.pixelXDimension << std::endl;
				out << "Pixel y dimension: " <<
					info.pixelYDimension << std::endl;

				out << "Components configuration: ";
				for (unsigned int i = 0;
					 i < sizeof(info.componentsConfiguration); ++i)
				{
					switch (info.componentsConfiguration[i])
					{
					case 0:
						out << "none";
						break;
					case 1:
						out << "Y";
						break;
					case 2:
						out << "Cb";
						break;
					case 3:
						out << "Cr";
						break;
					case 4:
						out << "R";
						break;
					case 5:
						out << "G";
						break;
					case 6:
						out << "B";
						break;
					default:
						out << "reserved";
					}
					out << ":";
				}
				out << std::endl;

				out << "Compressed bits per pixel: " <<
					info.compressedBitsPerPixel << std::endl;

				out << "Maker note: ";
				out <<
					std::string(info.makerNote.begin(),
								info.makerNote.end()) <<
					std::endl;

				out << "User comment: ";
				out << std::string(info.userComment.begin(),
								   info.userComment.end()) <<
					std::endl;

				out << "Related sound file: " <<
					info.relatedSoundFile << std::endl;

				out << "Date time original: " <<
					info.dateTimeOriginal << std::endl;
				out << "Date time digitized: " <<
					info.dateTimeDigitaized << std::endl;
				out << "subsec time: " <<
					info.subsecTime << std::endl;
				out << "subsec time original: " <<
					info.subsecTimeOriginal << std::endl;
				out << "subsec time digitaized: " <<
					info.subsecTimeDigitaized << std::endl;

				out << "Exposure time: " << info.exposureTime << std::endl;
				out << "Shutter speed: " << info.shutterSpeedValue << std::endl;
				out << "Aperture value: " << info.apertureValue << std::endl;
				out << "Brightness: " << info.brightnessValue << std::endl;
				out << "ExposureBias: " << info.exposureBiasValue << std::endl;
				out << "Max aperture: " << info.maxApertureValue << std::endl;
				out << "subject distance: " <<
					info.subjectDistance << std::endl;

				out << "Metering mode: ";
				switch (info.meteringMode)
				{
					case 0:
					{
						out << "unknown";
						break;
					}
					case 1:
					{
						out << "average";
						break;
					}

					case 2:
					{
						out << "center weighted average";
						break;
					}

					case 3:
					{
						out << "spot";
						break;
					}

					case 4:
					{
						out << "multi spot";
						break;
					}

					case 5:
					{
						out << "pattern";
						break;
					}

					case 6:
					{
						out << "partial";
						break;
					}

					case 255:
					{
						out << "other";
						break;
					}

					default:
					{
						out << "reserved";
						break;
					}
				}
				out << std::endl;

				out << "Light source: ";
				switch (info.lightSource)
				{
					case 0:
					{
						out << "unknown";
						break;
					}
					case 1:
					{
						out << "sun right";
						break;
					}
					case 2:
					{
						out << "fluorescent lamp";
						break;
					}
					case 3:
					{
						out << "tungsten";
						break;
					}
					case 17:
					{
						out << "default light A";
						break;
					}
					case 18:
					{
						out << "default light B";
						break;
					}
					case 19:
					{
						out << "default light C";
						break;
					}
					case 20:
					{
						out << "D55";
						break;
					}
					case 21:
					{
						out << "D65";
						break;
					}
					case 22:
					{
						out << "D75";
						break;
					}
					case 255:
					{
						out << "other";
						break;
					}
					default:
					{
						out << "reserved";
						break;
					}
				}
				out << std::endl;

				out << "flash: ";
				if (info.flash & 1)
				{
					out << "use flash, ";
					switch ((info.flash >> 1) & 3)
					{
						case 0:
							out << "not implement flash checker";
							break;
						case 1:
							out << "reserved.";
							break;
						case 2:
							out << "flash checker return false.";
							break;
						case 3:
							out << "flash checker return true.";
							break;
					}
				}
				else
				{
					out << "non use";
				}
				out << std::endl;


				out << "Focal length: " << info.focalLength << std::endl;
				out << "F number: " << info.fNumber << std::endl;

				out << "Exposure program: ";
				switch(info.exposureProgram)
				{
					case 0:
					{
						out << "undefined.";
						break;
					}
					case 1:
					{
						out << "manual";
						break;
					}
					case 2:
					{
						out << "normal program";
						break;
					}
					case 3:
					{
						out << "priority ";
						break;
					}
					case 4:
					{
						out << "priority shutter";
						break;
					}
					case 5:
					{
						out << "creative program";
						break;
					}
					case 6:
					{
						out << "action program";
						break;
					}
					case 7:
					{
						out << "portrait mode";
						break;
					}
					case 8:
					{
						out << "landscape mode";
						break;
					}
					default:
					{
						out << "reseved";
						break;
					}
				}
				out << std::endl;

				out << "Spectral sensitivity: " <<
					info.spectralSensitivity << std::endl;

				out << "ISO speed ratings: ";
				for (std::vector<unsigned short>::const_iterator itor =
						 info.isoSpeedRatings.begin();
					 itor != info.isoSpeedRatings.end();
					 ++itor)
				{
					out << (*itor) << ", ";
				}
				out << std::endl;

				out << "OECF: ";
				for (std::vector<unsigned char>::const_iterator itor =
						 info.OECF.begin();
					 itor != info.OECF.end();
					 ++itor)
				{
					out << (*itor) << ", ";
				}
				out << std::endl;

				std::cout << "Flash energy: " << info.flashEnergy << std::endl;

				out << "Spatial frequency response: ";
				for (std::vector<unsigned char>::const_iterator itor =
						 info.spatialFrequencyResponse.begin();
					 itor != info.spatialFrequencyResponse.end();
					 ++itor)
				{
					out << (*itor) << ", ";
				}
				out << std::endl;

				out << "Focal plane x resulution: " <<
					info.focalPlaneXResolution << std::endl;
				out << "Focal plane y resolution: " <<
					info.focalPlaneYResolution << std::endl;
				out << "Focal plane resolution unit: " <<
					info.focalPlaneResoutionUnit << std::endl;
				
				out << "Subject location: " << "(" << 
					info.subjectLocation[0] << ", " <<
					info.subjectLocation[1] << ")" << std::endl;

				out << "Exposure index: " << info.exposureIndex << std::endl;
				
				out << "Sensing method: ";
				switch (info.sensingMethod)
				{
					case 1:
					{
						out << "undefined.";
						break;
					}
					case 2:
					{
						out << "single panel color sensor";
						break;
					}
					case 3:
					{
						out << "dual panel color sensor";
						break;
					}
					case 4:
					{
						out << "triple panel color sensor";
						break;
					}
					case 5:
					{
						out << "color sequence sensor";
						break;
					}
					case 7:
					{
						out << "triple line linar sensor";
						break;
					}
					case 8:
					{
						out << "color sequence linar sensor";
						break;
					}
					default:
					{
						out << "reserved.";
						break;
					}
				}
				out << std::endl;

				out << "File source: " << info.fileSource << std::endl;
				out << "Scene type: " << info.sceneType << std::endl;
				out << "CFA pattern: ";
				for (std::vector<unsigned char>::const_iterator itor =
						 info.cfaPattern.begin();
					 itor != info.cfaPattern.end();
					 ++itor)
				{
					out << (*itor) << ", ";
				}
				out << std::endl;
					
				return out;
			}

		} exifDirectory;

		struct GPSDirectory
		{
			unsigned char versionID[4];
			std::string latitudeRef;
			Rational<unsigned int> latitude[3];
			std::string longitudeRef;
			Rational<unsigned int> longitude[3];
			unsigned char altitudeRef;
			Rational<unsigned int> altitude;
			Rational<unsigned int> timeStamp[3];
			std::string satellites;
			std::string status;
			std::string measureMode;
			Rational<unsigned int> DOP;
			std::string speedRef;
			Rational<unsigned int> speed;
			std::string trackRef;
			Rational<unsigned int> track;
			std::string imageDirectionRef;
			Rational<unsigned int> imageDirection;
			std::string mapDatum;
			std::string destinationLatitudeRef;
			Rational<unsigned int> destinationLatitude[3];
			std::string destinationLongitudeRef;
			Rational<unsigned int> destinationLongitude[3];
			std::string destinationBearingRef;
			Rational<unsigned int> destinationBearing;
			std::string destinationDistanceRef;
			Rational<unsigned int> destinationDistance;

			GPSDirectory():
				versionID(),
				latitudeRef(),
				latitude(),
				longitudeRef(),
				longitude(),
				altitudeRef(),
				altitude(),
				timeStamp(),
				satellites(),
				status(),
				measureMode(),
				DOP(),
				speedRef(),
				speed(),
				trackRef(),
				track(),
				imageDirectionRef(),
				imageDirection(),
				mapDatum(),
				destinationLatitudeRef(),
				destinationLatitude(),
				destinationLongitudeRef(),
				destinationLongitude(),
				destinationBearingRef(),
				destinationBearing(),
				destinationDistanceRef(),
				destinationDistance()
			{
				versionID[0] = 2;
				versionID[1] = 0;
				versionID[2] = 0;
				versionID[3] = 0;
			}

			GPSDirectory(const GPSDirectory& source):
				versionID(),
				latitudeRef(),
				latitude(),
				longitudeRef(),
				longitude(),
				altitudeRef(),
				altitude(),
				timeStamp(),
				satellites(),
				status(),
				measureMode(),
				DOP(),
				speedRef(),
				speed(),
				trackRef(),
				track(),
				imageDirectionRef(),
				imageDirection(),
				mapDatum(),
				destinationLatitudeRef(),
				destinationLatitude(),
				destinationLongitudeRef(),
				destinationLongitude(),
				destinationBearingRef(),
				destinationBearing(),
				destinationDistanceRef(),
				destinationDistance()				
			{
				std::copy(source.versionID,
						  source.versionID + sizeof(source.versionID),
						  versionID);
				latitudeRef = source.latitudeRef;
				std::copy(source.latitude,
						  source.latitude +
						  sizeof(source.latitude) /
						  sizeof(Rational<unsigned int>),
						  latitude);
				longitudeRef = source.longitudeRef;
				std::copy(source.longitude,
						  source.longitude +
						  sizeof(source.longitude) /
						  sizeof(Rational<unsigned int>),
						  longitude);
				altitudeRef = source.altitudeRef;
				altitude = source.altitude;
				std::copy(source.timeStamp,
						  source.timeStamp +
						  sizeof(source.timeStamp) /
						  sizeof(Rational<unsigned int>),
						  timeStamp);
				satellites = source.satellites;
				status = source.status;
				measureMode = source.measureMode;
				DOP = source.DOP;
				speedRef = source.speedRef;
				speed = source.speed;
				trackRef = source.trackRef;
				track = source.track;
				imageDirectionRef = source.imageDirectionRef;
				imageDirection = source.imageDirection;
				mapDatum = source.mapDatum;
				destinationLatitudeRef = source.destinationLatitudeRef;
				std::copy(source.destinationLatitude,
						  source.destinationLatitude + 
						  sizeof(source.destinationLatitude) /
						  sizeof(Rational<unsigned int>),
						  destinationLatitude);
				destinationLongitudeRef = source.destinationLongitudeRef;
				std::copy(source.destinationLongitude,
						  source.destinationLongitude +
						  sizeof(source.destinationLongitude) /
						  sizeof(Rational<unsigned int>),
						  destinationLongitude);
				destinationBearingRef = source.destinationBearingRef;
				destinationBearing = source.destinationBearing;
				destinationDistanceRef = source.destinationDistanceRef;
				destinationDistance = source.destinationDistance;
			}

			GPSDirectory& operator=(const GPSDirectory& source)
			{
				if (this == &source)
					return *this;

				std::copy(source.versionID,
						  source.versionID + sizeof(source.versionID),
						  versionID);
				latitudeRef = source.latitudeRef;
				std::copy(source.latitude,
						  source.latitude +
						  sizeof(source.latitude) /
						  sizeof(Rational<unsigned int>),
						  latitude);
				longitudeRef = source.longitudeRef;
				std::copy(source.longitude,
						  source.longitude +
						  sizeof(source.longitude) /
						  sizeof(Rational<unsigned int>),
						  longitude);
				altitudeRef = source.altitudeRef;
				altitude = source.altitude;
				std::copy(source.timeStamp,
						  source.timeStamp +
						  sizeof(source.timeStamp) /
						  sizeof(Rational<unsigned int>),
						  timeStamp);
				satellites = source.satellites;
				status = source.status;
				measureMode = source.measureMode;
				DOP = source.DOP;
				speedRef = source.speedRef;
				speed = source.speed;
				trackRef = source.trackRef;
				track = source.track;
				imageDirectionRef = source.imageDirectionRef;
				imageDirection = source.imageDirection;
				mapDatum = source.mapDatum;
				destinationLatitudeRef = source.destinationLatitudeRef;
				std::copy(source.destinationLatitude,
						  source.destinationLatitude + 
						  sizeof(source.destinationLatitude) /
						  sizeof(Rational<unsigned int>),
						  destinationLatitude);
				destinationLongitudeRef = source.destinationLongitudeRef;
				std::copy(source.destinationLongitude,
						  source.destinationLongitude +
						  sizeof(source.destinationLongitude) /
						  sizeof(Rational<unsigned int>),
						  destinationLongitude);
				destinationBearingRef = source.destinationBearingRef;
				destinationBearing = source.destinationBearing;
				destinationDistanceRef = source.destinationDistanceRef;
				destinationDistance = source.destinationDistance;

				return *this;
			}

			friend std::ostream& operator<<(
				std::ostream& out,
				const GPSDirectory& target)
			{
				out << "GPS version: " << 
					static_cast<unsigned short>(target.versionID[0]) << "." <<
					static_cast<unsigned short>(target.versionID[1]) << "." <<
					static_cast<unsigned short>(target.versionID[2]) << "." <<
					static_cast<unsigned short>(target.versionID[3]) <<
					std::endl;

				out << "latitudeRef: " <<  target.latitudeRef << std::endl;
				out << "latitude: " << 
					target.latitude[0] << ", " <<
					target.latitude[1] << ", " <<
					target.latitude[2] << std::endl;

				out << "longitudeRef: " <<  target.longitudeRef << std::endl;
				out << "longitude: " << 
					target.longitude[0] << ", " <<
					target.longitude[1] << ", " <<
					target.longitude[2] << std::endl;

				out << "altitudeRef: " << target.altitudeRef << std::endl;
				out << "altitude: " << target.altitude << std::endl;

				out << "(atomic clock's)timeStamp: " <<
					target.timeStamp[0] << ", " <<
					target.timeStamp[1] << ", " <<
					target.timeStamp[2] << std::endl;

				out << "satellites: " << target.satellites << std::endl;

				out << "GPS status: " << target.status << std::endl;

				out << "measure mode: " << target.measureMode << std::endl;

				out << "DOP: " << target.DOP << std::endl;

				out << "speedRef: " << target.speedRef << std::endl;
				out << "speed: " << target.speed << std::endl;

				out << "trackRef: " << target.trackRef << std::endl;
				out << "track: " << target.track << std::endl;

				out << "imageDirectionRef: " <<
					target.imageDirectionRef << std::endl;
				out << "iamgeDirection: " << target.imageDirection << std::endl;

				out << "map datum: " << target.mapDatum << std::endl;
				
				out << "destination latitude ref: " <<
					target.destinationLatitudeRef << std::endl;
				out << "destination latitude: " <<
					target.destinationLatitude[0] << ", " <<
					target.destinationLatitude[1] << ", " <<
					target.destinationLatitude[2] << std::endl;

				out << "destination longitude ref: " <<
					target.destinationLongitudeRef << std::endl;
				out << "destination longitude: " <<
					target.destinationLongitude[0] << ", " <<
					target.destinationLongitude[1] << ", " <<
					target.destinationLongitude[2] << std::endl;
				
				out << "destination bearing ref: " <<
					target.destinationBearingRef << std::endl;
				out << "destination Bearing: " <<
					target.destinationBearing << std::endl;

				out << "destination distance ref: " <<
					target.destinationDistanceRef << std::endl;
				out << "destination distance: " <<
					target.destinationDistance << std::endl;

				return out;
			}
		} gpsDirectory;

		ExifInfo()
			: imageWidth(),
			  imageHeight(),
			  bitsPerSample(),
			  compression(),
			  photometricInterpretation(),
			  orientation(),
			  samplesPerPixel(),
			  planarConfiguration(),
			  YCbCrSubSampling(),
			  YCbCrPositioning(),
			  resolutionUnit(),
			  xResolution(),
			  yResolution(),
			  stripOffsets(),
			  rowsPerStrip(),
			  stripByteCounts(),
			  JPEGInterchangeFormat(),
			  JPEGInterchangeFormatLength(),
			  transferFunction(),
			  whitePoint(),
			  primaryChromaticities(),
			  YCbCrCoefficients(),
			  referenceBlackWhite(),
			  dateTime(),
			  imageDescription(),
			  make(),
			  model(),
			  software(),
			  artist(),
			  copyright(),
			  exifDirectory(),
			  gpsDirectory()
		{}

		ExifInfo(const ExifInfo& source)
			: imageWidth(source.imageWidth),
			  imageHeight(source.imageHeight),
			  compression(source.compression),
			  photometricInterpretation(source.photometricInterpretation),
			  orientation(source.orientation),
			  samplesPerPixel(source.samplesPerPixel),
			  planarConfiguration(source.planarConfiguration),
			  YCbCrPositioning(source.YCbCrPositioning),
			  resolutionUnit(source.resolutionUnit),
			  xResolution(source.xResolution),
			  yResolution(source.yResolution),
			  stripOffsets(source.stripOffsets),
			  rowsPerStrip(source.rowsPerStrip),
			  stripByteCounts(source.stripByteCounts),
			  JPEGInterchangeFormat(source.JPEGInterchangeFormat),
			  JPEGInterchangeFormatLength(source.JPEGInterchangeFormatLength),
			  transferFunction(source.transferFunction),
			  dateTime(source.dateTime),
			  imageDescription(source.imageDescription),
			  make(source.make),
			  model(source.model),
			  software(source.software),
			  artist(source.artist),
			  copyright(source.copyright),
			  exifDirectory(source.exifDirectory),
			  gpsDirectory(source.gpsDirectory)
		{
			memcpy(bitsPerSample,
				   source.bitsPerSample,
				   sizeof(bitsPerSample));
			memcpy(YCbCrSubSampling,
				   source.YCbCrSubSampling,
				   sizeof(YCbCrSubSampling));
			memcpy(whitePoint,
				   source.whitePoint,
				   sizeof(source.whitePoint));
			memcpy(primaryChromaticities,
				   source.primaryChromaticities,
				   sizeof(primaryChromaticities));
			memcpy(YCbCrCoefficients,
				   source.YCbCrCoefficients,
				   sizeof(YCbCrCoefficients));
			memcpy(referenceBlackWhite,
				   source.referenceBlackWhite,
				   sizeof(referenceBlackWhite));

		}

		ExifInfo& operator=(const ExifInfo& source)
		{
			if (this == &source)
				return *this;

			imageWidth = source.imageWidth;
			imageHeight = source.imageHeight;
			compression = source.compression;
			photometricInterpretation = source.photometricInterpretation;
			orientation = source.orientation;
			samplesPerPixel = source.samplesPerPixel;
			planarConfiguration = source.planarConfiguration;
			YCbCrPositioning = source.YCbCrPositioning;
			resolutionUnit = source.resolutionUnit;
			xResolution = source.xResolution;
			yResolution = source.yResolution;
			stripOffsets = source.stripOffsets;
			rowsPerStrip = source.rowsPerStrip;
			stripByteCounts = source.stripByteCounts;
			JPEGInterchangeFormat = source.JPEGInterchangeFormat;
			JPEGInterchangeFormatLength = source.JPEGInterchangeFormatLength;
			transferFunction = source.transferFunction;
			dateTime = source.dateTime;
			imageDescription = source.imageDescription;
			make = source.make;
			model = source.model;
			software = source.software;
			artist = source.artist;
			copyright = source.copyright;

			memcpy(bitsPerSample,
				   source.bitsPerSample,
				   sizeof(bitsPerSample));
			memcpy(YCbCrSubSampling,
				   source.YCbCrSubSampling,
				   sizeof(YCbCrSubSampling));
			memcpy(whitePoint,
				   source.whitePoint,
				   sizeof(source.whitePoint));
			memcpy(primaryChromaticities,
				   source.primaryChromaticities,
				   sizeof(primaryChromaticities));
			memcpy(YCbCrCoefficients,
				   source.YCbCrCoefficients,
				   sizeof(YCbCrCoefficients));
			memcpy(referenceBlackWhite,
				   source.referenceBlackWhite,
				   sizeof(referenceBlackWhite));

			return *this;
		}

		friend std::ostream& operator<<(
			std::ostream& out,
			const ExifInfo& info)
		{
			// for image information
			out << "image width: " << info.imageWidth << std::endl;
			out << "imageHeight: " << info.imageHeight << std::endl;
			
			out << "bits per Sample: " <<
				info.bitsPerSample[0] << ":" <<
				info.bitsPerSample[1] << ":" <<
				info.bitsPerSample[2] << std::endl;

			out << "compression: ";
			switch (info.compression)
			{
				case 1:
					out << "no compression.";
					break;
				case 6:
					out << "JPEG compression.";
					break;
				default:
					out << "unknown.";
			}
			out << std::endl;

			out << "photometric interpretation: ";
			switch (info.photometricInterpretation)
			{
				case 2:
					out << "RGB";
					break;
				case 6:
					out << "YCbCr";
					break;
				default:
					out << "unknown";
			}
			out << std::endl;

			out << "orientation: ";
			switch(info.orientation)
			{
				case 1:
					out << "normal";
					break;
				case 2:
					out << "flip left and right";
					break;
				case 3:
					out << "flip top and bottom, flip left and right";
					break;
				case 4:
					out << "flip top and bottom";
					break;
				case 5:
					out << "rotate disclockwise pi/2, flip left and right";
					break;
				case 6:
					out << "rotate clockwise pi/2";
					break;
				case 7:
					out << "rotate clockwise pi/2, flip left and right";
					break;
				case 8:
					out << "rotate disclockwise pi/2";
					break;
				default:
					out << "unknown";
			}
			out << std::endl;

			out << "SamplesPerPixel: " << info.samplesPerPixel << std::endl;
			out << "planarConfiguration: ";
			if(info.planarConfiguration == 1)
			{
				out << "packed pixel";
			}
			else
			{
				out << "planared pixel";
			}
			out	<< std::endl;

			out << "YCbCrSubSampling: ";
			if (info.YCbCrSubSampling[0] == 2)
			{
				if (info.YCbCrSubSampling[1] == 1)
					out << "YCbCr = 4:2:2";
				else
					out << "YCbCr = 4:2:0";
			}
			else
				out << "unknown";	
			out<< std::endl;

			out << "YCbCrPositioning: ";
			if (info.YCbCrPositioning == 1)
				out << "center";
			else if (info.YCbCrPositioning == 2)
				out << "cosited";
			else
				out << "unknown";
			out << std::endl;

			out << "resolutionUnit: " << info.resolutionUnit <<
				"inch" << std::endl;

			out << "xResolution: " << info.xResolution << std::endl;
			out << "yResolution: " << info.yResolution << std::endl;

			// for image thumbnail data posision information.
			out << "stripOffsets: ";
			for (std::vector<unsigned int>::const_iterator itor =
					 info.stripOffsets.begin();
				 itor != info.stripOffsets.end();
				 ++itor)
			{
				out << (*itor) << ", ";
			}
			out << std::endl;

			out << "rowsPerStrip: " << info.rowsPerStrip << std::endl;
			out << "stripByteCounts: ";
			for (std::vector<unsigned int>::const_iterator itor =
					 info.stripByteCounts.begin();
				 itor != info.stripByteCounts.end();
				 ++itor)
			{
				out << (*itor) << ", ";
			}
			out << std::endl;

			out << "JPEGInterchangeFormat: " <<
				info.JPEGInterchangeFormat << std::endl;
			out << "JPEGInterchangeFormatLength: " <<
				info.JPEGInterchangeFormatLength << std::endl;

			// image data traits.
//			out << "TransferFunction: " << info.TransferFunction << std::endl;
// 			out << "whitePoint: " << info.whitePoint[2] << std::endl;
// 			out << "primaryChromaticities: " <<
//				info.primaryChromaticities[6] << std::endl;
			out << "YCbCrCoefficients: [" <<
				info.YCbCrCoefficients[0] << ", " <<
				info.YCbCrCoefficients[1] << ", " <<
				info.YCbCrCoefficients[2] << "]" << std::endl;
			out << "referenceBlackWhite: [" << 
				info.referenceBlackWhite[0] << ", " <<
				info.referenceBlackWhite[1] << ", " <<
				info.referenceBlackWhite[2] << ", " <<
				info.referenceBlackWhite[3] << ", " <<
				info.referenceBlackWhite[4] << ", " <<
				info.referenceBlackWhite[5] << "]" << std::endl;

			// other.
			out << "dateTime: " << info.dateTime.c_str() << std::endl;
			out << "imageDescription: " << 
				info.imageDescription.c_str() << std::endl;
			out << "make: " << info.make.c_str() << std::endl;
			out << "model: " << info.model.c_str() << std::endl;
			out << "software: " << info.software.c_str() << std::endl;
			out << "artist: " << info.artist.c_str() << std::endl;
			out << "copyright: " << info.copyright.c_str() << std::endl;
			
			out << info.exifDirectory << std::endl;
			out << info.gpsDirectory << std::endl;
			return out;
		}
	};

	/**
	 * Exifヘッダ解析器
	 */
	class ExifHeaderParser
	{
		friend class ExifTest;

	private:
		const void* parseTargetMemoryHead;
		const void* parseTargetMemoryTail;

		struct findFunctor
		{
			virtual bool operator()(const unsigned char*) const = 0;
			virtual ~findFunctor()  {}
		};

		struct findSOI : findFunctor
		{
			virtual bool operator()(const unsigned char* current) const
			{
				if (*current == 0xff &&
					*(current + 1) == 0xd8)
					return true;
				
				return false;
			}
		};

		struct findAPPn : findFunctor
		{
		private:
			const unsigned char findNumber;

		public:
			findAPPn(const unsigned char number)
				: findNumber(number)
			{}

			virtual bool operator()(const unsigned char* current) const
			{
				if (*current == 0xff &&
					*(current + 1) == (0xe0 + findNumber))
					return true;

				return false;
			}
		};

		const void*
		searchMarkerPos(const findFunctor& functor,
						const unsigned char* current = NULL,
						const unsigned char* tail = NULL)
		{
			if (current == NULL)
				current =
					reinterpret_cast<const unsigned char*>(
						parseTargetMemoryHead);

			if (tail == NULL)
				tail =
					reinterpret_cast<const unsigned char*>(
						parseTargetMemoryTail);
				

			for (; (current + 1) != tail; ++current)
			{
				if (functor(current))
					return current;
			}

			return NULL;
		}

		bool isExifMarker(const unsigned char* current)
		{
			if (*current == 'E' &&
				*(current + 1) == 'x' &&
				*(current + 2) == 'i' &&
				*(current + 3) == 'f' &&
				*(current + 4) == 0 &&
				*(current + 5) == 0)
				return true;

			return false;
		}

		const void* getTiffHeaderPos()
		{
			assert(parseTargetMemoryHead != NULL);

			const void* SOIPos = searchMarkerPos(findSOI());
			if (SOIPos == NULL)
				throw std::runtime_error("input data is invalid.");

			const void* APP1Pos =
				searchMarkerPos(findAPPn(1), 
								reinterpret_cast<const unsigned char*>
								(SOIPos));

			if (APP1Pos == NULL)
				throw std::runtime_error("input data is invalid.");

			const unsigned char* parsePos =
				reinterpret_cast<const unsigned char*>(APP1Pos) + 2;

			EndianConverter converter;
			const unsigned short tiffHeaderSize =
				converter.fromBig(
					*reinterpret_cast<const unsigned short*>(parsePos));
			
			// shift APP1 end position.
			parseTargetMemoryTail = 
				reinterpret_cast<const void*>(
					reinterpret_cast<const unsigned char*>(parsePos) +
					tiffHeaderSize);

			if (tiffHeaderSize < 2)
				throw std::runtime_error("input data is invalid.");

			// offset shift to "Exif\0\0" Header position.
			parsePos += 2;

			if (!isExifMarker(parsePos))
				throw std::runtime_error("input data is invalid.");

			// offset shift to TIFF header position.
			parsePos += 6;

			return parsePos;
		}

	public:
		ExifHeaderParser(const void* parseTargetMemoryHead_,
						 const void* parseTargetMemoryTail_)
			: parseTargetMemoryHead(parseTargetMemoryHead_),
			  parseTargetMemoryTail(parseTargetMemoryTail_)
		{
			if (parseTargetMemoryHead == NULL ||
				parseTargetMemoryTail == NULL ||
				parseTargetMemoryTail <= parseTargetMemoryHead)
				throw std::invalid_argument(
					"parseTargetMemory area must be null.");
		}

		bool isValidHeader()
		{
			try
			{
				parseTargetMemoryHead = getTiffHeaderPos();
			}
			catch (std::exception& /*e*/)
			{
				return false;
			}

			return true;
		}

		const void* getTiffHeaderBegin() const
		{
			return parseTargetMemoryHead;
		}
		const void* getTiffHeaderEnd() const
		{
			return parseTargetMemoryTail;
		}
	};

	/**
	 * Tiffヘッダディレクトリ解析器
	 */
	class DirectoryParser
	{
	private:
		const void* head;
		const void* tail;
		UserDefinedEndianConverter& converter;
		std::vector<TagInfo> infos;

		unsigned short countEntries()
		{
			unsigned short count =
				converter.from(
					*reinterpret_cast<const unsigned short*>(head));

			if ((reinterpret_cast<const unsigned char*>(tail) -
				 reinterpret_cast<const unsigned char*>(head)) <
				count)
				throw std::runtime_error("header's entry count is invalid.");

			return count;
		}

		DirectoryParser(const DirectoryParser&);
		DirectoryParser& operator=(const DirectoryParser&);
	public:

		DirectoryParser(const void* head_, const void* tail_,
						UserDefinedEndianConverter& converter_)
			
			: head(head_), tail(tail_),
			  converter(converter_),
			  infos()
		{
			if (head == NULL ||
				tail == NULL || 
				head > tail)
				throw std::invalid_argument(
					"argument pointer must be not null.");

			const unsigned short count = countEntries();
			infos.clear();

			assert(((const unsigned char*)(tail) -
					(const unsigned char*)(head)) / 12 >= count);

			for (unsigned short index = 0; index < count; ++index)
				infos.push_back(
					TagInfo::parseMemoryToTagInfo(
						reinterpret_cast<const unsigned char*>(head) +
						sizeof(unsigned short) +
						+ (12 * index),
						converter));
		}

		typedef std::vector<TagInfo>::iterator iterator;
		typedef std::vector<TagInfo>::const_iterator const_iterator;

		iterator begin()
		{
			return infos.begin();
		}

		iterator end()
		{
			return infos.end();
		}

		const_iterator begin() const
		{
			return infos.begin();
		}

		const_iterator end() const
		{
			return infos.end();
		}
	
	};

	/**
	 * Tiffヘッダ解析器
	 */
	class TiffHeaderParser
	{
	private:
		TiffHeaderParser(const TiffHeaderParser&);
		TiffHeaderParser& operator=(const TiffHeaderParser&);

		const void* parseTargetMemoryHead;
		const void* parseTargetMemoryTail;

		UserDefinedEndianConverter* converter;

		std::vector<TagInfo> topDirectoryEntries;

		void parse0thImageFileDirectory(const void* directoryHead)
		{
			DirectoryParser parser(directoryHead,
								   parseTargetMemoryTail,
								   *converter);

			topDirectoryEntries.clear();

			std::copy(parser.begin(), parser.end(),
					  std::back_inserter(topDirectoryEntries));
		}

		bool isLittleEndian()
		{
			assert(parseTargetMemoryHead != NULL);
			if (reinterpret_cast<const unsigned char*>(parseTargetMemoryTail) -
				reinterpret_cast<const unsigned char*>(parseTargetMemoryHead) < 2)
				throw std::runtime_error("TIFF header is invalid.");

			const char* head =
				reinterpret_cast<const char*>(parseTargetMemoryHead);
			if (head[0] != head[1])
				throw std::runtime_error(
					"TIFF header's endian mark is invalid.");
				
			if (head[0] == 'M')
				return false;
			else if (head[0] == 'I')
				return true;
			else
				throw std::runtime_error(
					"TIFF header's endian mark is invalid.");
		}

	public:
		TiffHeaderParser(const void* head, const void* tail)
			: parseTargetMemoryHead(head),
			  parseTargetMemoryTail(tail),
			  converter(NULL),
			  topDirectoryEntries()
		{
			if (parseTargetMemoryHead == NULL ||
				parseTargetMemoryTail == NULL ||
				parseTargetMemoryTail < parseTargetMemoryHead)
				throw std::invalid_argument(
					"argument parseTargetMemory must be not null.");
		}

		~TiffHeaderParser()
		{
			if (converter != NULL)
				delete converter;
		}

		std::vector<TagInfo>
		parseImageFileDirectory(const size_t directoryOffset)
		{
 			DirectoryParser parser(
 				reinterpret_cast<const char*>(parseTargetMemoryHead) +
 				directoryOffset,
 				parseTargetMemoryTail,
				*converter);

 			std::vector<TagInfo> IFDEntries;
 			std::copy(parser.begin(), parser.end(),
 					  std::back_inserter(IFDEntries));

			return IFDEntries;
		}

		const UserDefinedEndianConverter& getEndianConverter() const
		{
			return *converter;
		}

		bool isValidHeader()
		{
			assert(parseTargetMemoryHead != NULL);

			if (reinterpret_cast<const unsigned char*>(parseTargetMemoryTail) -
				reinterpret_cast<const unsigned char*>(parseTargetMemoryHead) <
				12)
				return false;

			try
			{
				const EndianType endian =
					isLittleEndian() ? littleEndian : bigEndian;
				converter =	new UserDefinedEndianConverter(endian);

				// TIFF identifier code
				if (converter->from(*(reinterpret_cast<const unsigned short*>(
										  parseTargetMemoryHead) + 1)) !=
					0x002a)
					return false;

				// 0 th IFD pointer
				const size_t IFDOffset =
					converter->from(*(reinterpret_cast<const unsigned int*>(
										  parseTargetMemoryHead) + 1));
				if (IFDOffset < 8)
					return false;

				// calc entry offset posision.
				const void* directoryHead =
					static_cast<const void*>(
						reinterpret_cast<const unsigned char*>(
							parseTargetMemoryHead) + IFDOffset);

				parse0thImageFileDirectory(directoryHead);

			}
			catch (std::exception& /*e*/)
			{
				return false;
			}

			return true;
		}

		typedef std::vector<TagInfo>::iterator iterator;
		typedef std::vector<TagInfo>::const_iterator const_iterator;

		const_iterator begin() const
		{
			return topDirectoryEntries.begin();
		}

		const_iterator end() const
		{
			return topDirectoryEntries.end();
		}

		iterator begin()
		{
			return topDirectoryEntries.begin();
		}

		iterator end()
		{
			return topDirectoryEntries.end();
		}
	};

	/**
	 * タグ名解析器
	 */
	class TagNameResolver
	{
	private:
		TagInfoParser& parser;
		TiffHeaderParser& tiffParser;

		struct CountTrue
		{
			bool operator()(size_t)
			{
				return true;
			}
		};

		struct CountOnce
		{
			bool operator()(size_t count)
			{
				return count == 1;
			}
		};

		struct CountAny
		{
		private:
			const size_t any;
		public:
			CountAny(const size_t any_)
				: any(any_)
			{}

			bool operator()(size_t count)
			{
				return count == any;
			}
		};
		
		struct CountRange
		{
		private:
			const size_t smallSide;
			const size_t bigSide;
		public:
			CountRange(const size_t small_,
					   const size_t big_)
				: smallSide(small_), bigSide(big_)
			{
				assert(smallSide <= bigSide);
			}

			bool operator()(size_t count)
			{
				if (smallSide <= count &&
					bigSide >= count)
					return true;
				return false;
			}
		};

		template <class CountCheckFunctor>
		std::vector<unsigned int>
		getShortOrLong(const TagInfo& tag,
					   CountCheckFunctor inCount)
		{
			if (!inCount(tag.getNumberOfValues()))
				throw std::runtime_error(
					"image width count is invalid.");

			if (tag.getTagType() == 3)
			{
				std::vector<unsigned short> usv =
					parser.getUnsignedShorts(tag);
				std::vector<unsigned int> result;
				std::copy(usv.begin(), usv.end(),
						  std::back_inserter(result));
				return result;
			}
			else if (tag.getTagType() == 4)
				return
					parser.getUnsignedLongs(tag);
			else
				throw std::runtime_error(
					"tag type is not ushort or ulong.");
		}

		unsigned short SingleUShortDataRead(
			const TagInfo& tag,
			const std::string& errorMessage)
		{
			std::vector<unsigned short> values =
				parser.getUnsignedShorts(tag);
							
			if (values.size() != 1)
				throw std::runtime_error(
					errorMessage.c_str());

			return values[0];
		}

		unsigned int SingleULongDataRead(
			const TagInfo& tag,
			const std::string& errorMessage)
		{
			std::vector<unsigned int> values =
				parser.getUnsignedLongs(tag);
							
			if (values.size() != 1)
				throw std::runtime_error(
					errorMessage.c_str());

			return values[0];
		}

		int SingleLongDataRead(
			const TagInfo& tag,
			const std::string& errorMessage)
		{
			std::vector<int> values =
				parser.getLongs(tag);
							
			if (values.size() != 1)
				throw std::runtime_error(
					errorMessage.c_str());

			return values[0];
		}

		Rational<unsigned int> SingleURationalDataRead(
			const TagInfo& tag,
			const std::string& errorMessage)
		{
			std::vector<Rational<unsigned int> >
				values =
				parser.getUnsignedRationals(tag);
							
			if (values.size() != 1)
				throw std::runtime_error(
					errorMessage.c_str());

			return values[0];
		}

		Rational<int> SingleRationalDataRead(
			const TagInfo& tag,
			const std::string& errorMessage)
		{
			std::vector<Rational<int> >
				values =
				parser.getRationals(tag);
							
			if (values.size() != 1)
				throw std::runtime_error(
					errorMessage.c_str());

			return values[0];
		}

	public:
		TagNameResolver(TagInfoParser& parser_,
						TiffHeaderParser& tiffParser_)
			: parser(parser_),
			  tiffParser(tiffParser_)
		{}

		template <typename Iterator>
		ExifInfo::GPSDirectory
		getGPSDirectory(Iterator head,
						Iterator tail)
		{
			ExifInfo::GPSDirectory directory;
			
			for (; head != tail; ++head)
			{
				switch (head->getTag())
				{
					case 0x00:
					{
						std::vector<unsigned char> version = 
							parser.getBytes(*head);
						if (version.size() != 4)
							throw std::runtime_error("version tag invalid.");

						std::copy(version.begin(),
								  version.end(),
								  directory.versionID);
					}
					break;

					case 0x01:
					{
						directory.latitudeRef = parser.getString(*head);
					}
					break;

					case 0x02:
					{
						std::vector<Rational<unsigned int> > latitude =
							parser.getUnsignedRationals(*head);
						if (latitude.size() != 3)
							throw std::runtime_error("latitude tag invalid.");

						std::copy(latitude.begin(),
								  latitude.end(),
								  directory.latitude);
					}
					break;

					case 0x03:
					{
						directory.longitudeRef = parser.getString(*head);
					}
					break;

					case 0x04:
					{
						std::vector<Rational<unsigned int> > longitude =
							parser.getUnsignedRationals(*head);
						if (longitude.size() != 3)
							throw std::runtime_error("longitude tag invalid.");

						std::copy(longitude.begin(),
								  longitude.end(),
								  directory.longitude);
					}
					break;

					case 0x05:
					{
						std::vector<unsigned char> altitudeRef = 
							parser.getBytes(*head);

						if (altitudeRef.size() != 1)
							throw std::runtime_error(
								"altitudeRef tag invalid.");

						directory.altitudeRef = altitudeRef[0];
					}
					break;

					case 0x06:
					{
						directory.altitude =
							SingleURationalDataRead(*head,
													"altitude tag invalid.");
					}
					break;

					case 0x07:
					{
						std::vector<Rational<unsigned int> > timeStamp =
							parser.getUnsignedRationals(*head);
						if (timeStamp.size() != 3)
							throw std::runtime_error("timeStamp tag invalid.");

						std::copy(timeStamp.begin(),
								  timeStamp.end(),
								  directory.timeStamp);
					}
					break;

					case 0x08:
					{
						directory.satellites =
							parser.getString(*head);
					}
					break;

					case 0x09:
					{
						directory.status =
							parser.getString(*head);
					}
					break;

					case 0x0a:
					{
						directory.measureMode =
							parser.getString(*head);
					}
					break;

					case 0x0b:
					{
						directory.DOP =
							SingleURationalDataRead(*head, "DOP tag invalid.");
					}
					break;

					case 0x0c:
					{
						directory.speedRef =
							parser.getString(*head);
					}
					break;

					case 0x0d:
					{
						directory.speed =
							SingleURationalDataRead(*head,
													"speed tag invalid.");
					}
					break;

					case 0x0e:
					{
						directory.trackRef =
							parser.getString(*head);
					}
					break;

					case 0x0f:
					{
						directory.track =
							SingleURationalDataRead(*head,
													"track tag invalid.");
					}
					break;

					case 0x10:
					{
						directory.imageDirectionRef = parser.getString(*head);
					}
					break;

					case 0x11:
					{
						directory.imageDirection =
							SingleURationalDataRead(
								*head, "imgDirection tag invalid.");
					}
					break;

					case 0x12:
					{
						directory.mapDatum = parser.getString(*head);
					}
					break;

					case 0x13:
					{
						directory.destinationLatitudeRef =
							parser.getString(*head);
					}
					break;

					case 0x14:
					{
						std::vector<Rational<unsigned int> >
							destinationLatitude =
							parser.getUnsignedRationals(*head);
						if (destinationLatitude.size() != 3)
							throw std::runtime_error(
								"destLatitude tag invalid.");

						std::copy(destinationLatitude.begin(),
								  destinationLatitude.end(),
								  directory.destinationLatitude);
					}
					break;

					case 0x15:
					{
						directory.destinationLongitudeRef =
							parser.getString(*head);
					}
					break;

					case 0x16:
					{
						std::vector<Rational<unsigned int> >
							destinationLongitude =
							parser.getUnsignedRationals(*head);
						if (destinationLongitude.size() != 3)
							throw std::runtime_error(
								"destLongitude tag invalid.");

						std::copy(destinationLongitude.begin(),
								  destinationLongitude.end(),
								  directory.destinationLongitude);
					}
					break;

					case 0x17:
					{
						directory.destinationBearingRef =
							parser.getString(*head);
					}
					break;

					case 0x18:
					{
						directory.destinationBearing =
							SingleURationalDataRead(
								*head, "destBearing tag invalid.");
					}
					break;

					case 0x19:
					{
						directory.destinationDistanceRef =
							parser.getString(*head);
					}
					break;

					case 0x1a:
					{
						directory.destinationDistance =
							SingleURationalDataRead(
								*head, "destDistance tag invalid.");
					}
					break;
				}
			}

			return directory;
		}

		template <typename Iterator>
		ExifInfo::ExifDirectory
		getExifDirectory(Iterator head,
						 Iterator tail)
		{
			ExifInfo::ExifDirectory directory;

			for (; head != tail; ++head)
			{
				switch (head->getTag())
				{
					case 0x9000: // exif version
					{
						std::vector<unsigned char> version = 
							parser.getBytes(*head);
						if (version.size() != 4)
							throw std::runtime_error("version tag invalid.");

						for (size_t offset = 0;
							 offset < sizeof(directory.exifVersion);
							 ++offset)
						{
							directory.exifVersion[offset] =
								version[offset];
						}
						break;
					}

					case 0xa000: // flashpix version
					{
						std::vector<unsigned char> version = 
							parser.getBytes(*head);
						if (version.size() != 4)
							throw std::runtime_error("version tag invalid.");

						for (size_t offset = 0;
							 offset < sizeof(directory.flashPixVersion);
							 ++offset)
						{
							directory.flashPixVersion[offset] =
								version[offset];
						}
						break;
					}
						
					case 0xa001: // color space
					{
						directory.colorSpace = 
							SingleUShortDataRead(*head, "color space tag"
												 " invalid");
						break;
					}

					case 0x9101: // components configuration
					{
						std::vector<unsigned char> componentsConfig = 
							parser.getBytes(*head);
						if (componentsConfig.size() != 4)
							throw std::runtime_error("components configuration"
													 "tag invalid.");

						for (size_t offset = 0;
							 offset < sizeof(directory.componentsConfiguration);
							 ++offset)
						{
							directory.componentsConfiguration[offset] =
								componentsConfig[offset];
						}
						break;
					}

					case 0x9102: // compressed bits per pixel
					{
						directory.compressedBitsPerPixel =
							SingleURationalDataRead(
								*head, "compressed bits per "
								"pixel tag invalid.");
						break;
					}

					case 0xa002: // pixel x dimension
					{
						directory.pixelXDimension =
							getShortOrLong(*head, CountOnce())[0];
						break;
					}
					case 0xa003: // pixel y dimension
					{
						directory.pixelYDimension =
							getShortOrLong(*head, CountOnce())[0];
						break;
					}

					case 0x927c: // maker note
					{
						directory.makerNote =
							parser.getBytes(*head);
						break;
					}

					case 0x9286: // user comment
					{
						directory.userComment =
							parser.getBytes(*head);
						break;
					}

					case 0xa004: // related sound file
					{
						directory.relatedSoundFile =
							parser.getString(*head);
						break;
					}

					case 0x9003: // date time original
					{
						directory.dateTimeOriginal =
							parser.getString(*head);
						break;
					}

					case 0x9004: // date time digitized
					{
						directory.dateTimeDigitaized =
							parser.getString(*head);
						break;
					}

					case 0x9290: // sub sec time
					{
						directory.subsecTime =
							parser.getString(*head);
						break;
					}

					case 0x9291: // sub sec time original
					{
						directory.subsecTimeOriginal =
							parser.getString(*head);
						break;
					}

					case 0x9292: // sub sec time digitized
					{
						directory.subsecTimeDigitaized =
							parser.getString(*head);
						break;
					}

					case 0x829a: // exposure time
					{
						directory.exposureTime =
							SingleURationalDataRead(
								*head, "exposure time tag invalid.");
						break;
					}

					case 0x829d: // F number
					{
						directory.fNumber =
							SingleURationalDataRead(*head,
													"F number tag invalid.");
						break;
					}

					case 0x8822: // exposure program
					{
						directory.exposureProgram =
							SingleUShortDataRead(
								*head, "exposure program tag invalid.");
						break;
					}

					case 0x8824: // spectral sensitivity
					{
						directory.spectralSensitivity =
							parser.getString(*head);
						break;
					}

					case 0x8827: // ISO speed ratings
					{
						directory.isoSpeedRatings =
							parser.getUnsignedShorts(*head);
						break;
					}

					case 0x8828: // OECF
					{
						directory.OECF =
							parser.getBytes(*head);
						break;
					}

					case 0x9201: // shutter speed value
					{
						directory.shutterSpeedValue =
							SingleRationalDataRead(*head,
												   "shutter speed value "
												   "tag invalid.");
						break;
					}

					case 0x9202: // aperture value
					{
						directory.apertureValue =
							SingleURationalDataRead(
								*head, "aperture value tag invalid.");
						break;
					}

					case 0x9203: // brightness value
					{
						directory.brightnessValue =
							SingleRationalDataRead(
								*head, "brightness value tag invalid.");
						break;
					}

					case 0x9204: // exposure bias value
					{
						directory.exposureBiasValue =
							SingleRationalDataRead(*head,
													"exposure bias value tag "
												   "invalid.");
						break;
					}


					case 0x9205: // max aperture value
					{
						directory.maxApertureValue =
							SingleURationalDataRead(*head, "max aperture "
													"value tag invalid.");
						break;
					}

					case 0x9206: // subject distance
					{
						directory.subjectDistance =
							SingleURationalDataRead(
								*head, "subject distance tag invalid.");
						break;
					}

					case 0x9207: // metering mode
					{
						directory.meteringMode =
							SingleUShortDataRead(*head,
												 "metering mode tag invalid.");
						break;
					}

					case 0x9208: // light source
					{
						directory.lightSource =
							SingleUShortDataRead(*head,
												 "light source tag invalid.");
						break;
					}

					case 0x9209: // flash
					{
						directory.flash =
							SingleUShortDataRead(*head,
												 "flash tag invalid.");
						break;
					}

					case 0x920a: // focal length
					{
						directory.focalLength =
							SingleURationalDataRead(
								*head, "focal length tag invalid.");
						break;
					}

					case 0xa20b: // flash energy
					{
						directory.flashEnergy =
							SingleURationalDataRead(
								*head, "flash energy tag invalid.");
						break;
					}

					case 0xa20c: // spatial frequency response
					{
						directory.spatialFrequencyResponse =
							parser.getBytes(*head);
						break;
					}

					case 0xa20e: // focal plane x resolution
					{
						directory.focalPlaneXResolution =
							SingleURationalDataRead(*head,
													"focal plane x resolution"
													" tag invalid.");
						break;
					}

					case 0xa20f: // focal plane y resolution
					{
						directory.focalPlaneYResolution =
							SingleURationalDataRead(*head,
													"focal plane y resolution"
													" tag invalid.");
						break;
					}

					case 0xa210: // focal plane resolution unit
					{
						directory.focalPlaneResoutionUnit =
							SingleUShortDataRead(*head,
												 "focal plane resolution unit"
												 " tag invalid.");
						break;
					}

					case 0xa214: // subject location
					{
						std::vector<unsigned short> location =
							parser.getUnsignedShorts(*head);
						if (location.size() != 2)
							throw std::runtime_error("subject localtion tag "
													 "invalid.");

						directory.subjectLocation[0] =
							location[0];
						directory.subjectLocation[1] =
							location[1];
						break;
					}

					case 0xa215: // exposure index
					{
						directory.exposureIndex =
							SingleURationalDataRead(
								*head, "exposure index tag invalid.");
						break;
					}

					case 0xa217: // sensing method
					{
						directory.sensingMethod =
							SingleUShortDataRead(
								*head, "sensing method tag invalid.");
						break;
					}

					case 0xa300: // file source
					{
						directory.fileSource =
							parser.getBytes(*head)[0];
						break;
					}

					case 0xa301: // scene type
					{
						directory.sceneType =
							parser.getBytes(*head)[0];
						break;
					}

					case 0xa302: // cfa pattern
					{
						directory.cfaPattern =
							parser.getBytes(*head);
						break;
					}

					case 0xa005: // interoperability IFD pointer
					{
						directory.interoperabilityIFDPointer =
							SingleULongDataRead(*head, "interoperability IFD "
												"pointer tag invalid.");
						break;
					}
				}
			}

			return directory;
		}

		ExifInfo getExifInfo(std::vector<TagInfo>& tags)
		{
			return getExifInfo(tags.begin(), tags.end());
		}

		template <typename Iterator>
		ExifInfo getExifInfo(Iterator head,
							 Iterator tail)
		{
			ExifInfo info;

			for (; head != tail; ++head)
			{
				switch (head->getTag())
				{
					// image data tags.
					case 0x0100: // image width
					{
						info.imageWidth =
							getShortOrLong(*head, CountOnce())[0];
						break;
					}

					case 0x0101: // image height
					{
						info.imageHeight =
							getShortOrLong(*head, CountOnce())[0];
						break;
					}

					case 0x0102: // bits per sample
					{
						std::vector<unsigned short> shorts =
							parser.getUnsignedShorts(*head);

						if (shorts.size() != 3)
							throw std::runtime_error(
								"bits per sample tag data count is not 3.");

						info.bitsPerSample[0] = shorts[0];
						info.bitsPerSample[1] = shorts[1];
						info.bitsPerSample[2] = shorts[2];
						break;
					}

					case 0x0103: // compression
					{
						info.compression =
							SingleUShortDataRead(
								*head, "compression tag data count is not 1.");

						break;
					}

					case 0x0106: //photometric interpretation
					{
						info.photometricInterpretation =
							SingleUShortDataRead(
								*head, "photometric interpretation "
								"tag data count is not 1.");

						break;
					}
					case 0x0112: // orientation
					{
						info.orientation =
							SingleUShortDataRead(
								*head, "orientation tag data count is not 1.");
						break;
					}
					case 0x0115: // samples per pixel
					{
						info.samplesPerPixel =
							SingleUShortDataRead(
								*head, "samples per pixel tag data "
								"count is not 1.");
						break;
					}
					case 0x011c: // planar configuration
					{
						info.planarConfiguration =
							SingleUShortDataRead(
								*head, "planar configuration tag data "
								"count is not 1.");
						break;
					}
					case 0x0212: // YCbCr subsampling
					{
						std::vector<unsigned short> value =
							parser.getUnsignedShorts(*head);

						if (value.size() != 2)
							throw std::runtime_error(
								"YCbCr subsampling tag data "
								"count is not 1.");

						for (unsigned int index = 0;
							 index < value.size(); ++index)
							info.YCbCrSubSampling[index] =
								value[index];
						break;

					}
					case 0x0213: // YCbCr positioning
					{
						info.YCbCrPositioning =
							SingleUShortDataRead(
								*head, "YCbCr positioning tag data "
								"count is not 1.");
						break;
					}
					case 0x011a: // X resolution
					{
						info.xResolution =
							SingleURationalDataRead(
								*head, "X resolution tag data "
								"count is not 1.");
						break;
					}
					case 0x011b: // Y resolution
					{
						info.yResolution =
							SingleURationalDataRead(
								*head, "Y resolution tag data "
								"count is not 1.");
						break;
					}
					case 0x0128: // resolution unit
					{
						info.resolutionUnit =
							SingleUShortDataRead(
								*head, "resolution unit tag data "
								"count is not 1.");
						break;
					}
					
					// image memory pos.
					case 0x0111: // strip offsets (short or long)
					{
						info.stripOffsets =
							getShortOrLong(*head, CountTrue());
						break;
					}
					case 0x0116: // rows per strip (short or long)
					{
						info.rowsPerStrip =
							getShortOrLong(*head, CountOnce())[0];
						break;
					}
					case 0x0117: // strip byte counts (short or long)
					{
						info.stripByteCounts =
							getShortOrLong(*head, CountTrue());
						break;
					}
					case 0x0201: // jpeg interchange format (long)
					{
						info.JPEGInterchangeFormat =
							SingleULongDataRead(
								*head, "JPEG interchange format tag data "
								"count is not 1.");
						break;
					}
					case 0x0202: // jpeg interchange format length (long)
					{
						info.JPEGInterchangeFormatLength =
							SingleULongDataRead(
								*head, "JPEG interchange format length tag "
								"data count is not 1.");
						break;
					}

					// image data traits
					case 0x012d: // transfer function (short)
					{
						info.transferFunction =
							parser.getUnsignedShorts(*head);
					   
						if (info.transferFunction.size() != 3 * 256)
							throw std::runtime_error(
								"transfer function parameter is not "
								"3 * 256 count.");
						break;
					}
					case 0x013e: // white point (rational)
					{
						std::vector<Rational<unsigned int> > value = 
							parser.getUnsignedRationals(*head);

						if (value.size() != 2)
							throw std::runtime_error(
								"white point count is not 2.");

						info.whitePoint[0] = value[0];
						info.whitePoint[1] = value[1];

						break;
					}
					case 0x013f: // primary chromaticities (rational)
					{
						std::vector<Rational<unsigned int> > value =
							parser.getUnsignedRationals(*head);

						if (value.size() != 6)
							throw std::runtime_error(
								"primary chromaticities count is not 6.");

						for (unsigned int index = 0;
							 index < value.size(); ++index)
							info.primaryChromaticities[index] =
								value[index];

						break;
					}
					case 0x0211: // YCbCr coefficients (rational)
					{
						std::vector<Rational<unsigned int> > value = 
							parser.getUnsignedRationals(*head);

						if (value.size() != 3)
							throw std::runtime_error(
								"YCbCr coefficients count is not 3.");

						for (unsigned int index = 0;
							 index < value.size(); ++index)
							info.YCbCrCoefficients[index] =
								value[index];

						break;
					}

					case 0x0214: // reference black white (rational)
					{
						std::vector<Rational<unsigned int> > value = 
							parser.getUnsignedRationals(*head);

						if (value.size() != 6)
							throw std::runtime_error(
								"reference black white count is not 6.");

						break;
					}

					// other information.
					case 0x0132: // datetime (asciz)
					{
						info.dateTime =
							parser.getString(*head);

						break;
					}
					case 0x010e: // image description (asciz)
					{
						info.imageDescription =
							parser.getString(*head);

						break;
					}
					case 0x010f: // make (asciz)
					{
						info.make =
							parser.getString(*head);

						break;
					}
					case 0x0110: // model (asciz)
					{
						info.model =
							parser.getString(*head);

						break;
					}
					case 0x0131: // software (asciz)
					{
						info.software =
							parser.getString(*head);

						break;
					}
					case 0x013b: // artist (asciz)
					{
						info.artist =
							parser.getString(*head);

						break;
					}
					case 0x8298: // copyright (asciz)
					{
						info.copyright =
							parser.getString(*head);

						break;
					}
					
					case 0x8769: // Exif IFD
					{
						std::vector<unsigned int> 
							result = parser.getUnsignedLongs(*head);
						std::vector<TagInfo> ifdEntries = 
							tiffParser.parseImageFileDirectory(result[0]);

						info.exifDirectory =
							getExifDirectory(ifdEntries.begin(),
											 ifdEntries.end());
						break;
					}

					case 0x8825: // GPS IFD
					{
						std::vector<unsigned int> 
							result = parser.getUnsignedLongs(*head);
						std::vector<TagInfo> ifdEntries = 
							tiffParser.parseImageFileDirectory(result[0]);
						std::cout << "gps entries: " <<
							ifdEntries.size() << std::endl;

 						info.gpsDirectory =
 							getGPSDirectory(ifdEntries.begin(),
											ifdEntries.end());
						break;
					}
					
					default:
					{
						std::cout << "unknown header tag" << std::endl;
						std::cout << head->getTag() << std::endl;
					}
				}
			}

			return info;
		}
	};

};
#endif /* EXIF_HPP_ */
