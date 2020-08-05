#include "jni_envelope.h"
#include "android_util.h"

static const std::string JNI_LOGGER_CLASS = "JniLog";

const JavaPrimitiveType JavaPrimitiveType::Void("V");
const JavaPrimitiveType JavaPrimitiveType::Int("I");
const JavaPrimitiveType JavaPrimitiveType::Long("J");
const JavaPrimitiveType JavaPrimitiveType::Boolean("Z");
const JavaPrimitiveType JavaPrimitiveType::Float("F");

JavaType::JavaType() {
}

JavaType::~JavaType() {
}

JavaArray::JavaArray(const JavaType &base) : myBase(base) {
}

std::string JavaArray::code() const {
	return "[" + myBase.code();
}

JavaClass::JavaClass(const std::string &name) : myName(name) {
	myClass = 0;
}

JavaClass::~JavaClass() {
	if (myClass != 0) {
		AndroidUtil::getEnv()->DeleteGlobalRef(myClass);
	}
}

std::string JavaClass::code() const {
	return "L" + myName + ";";
}

jclass JavaClass::j() const {
	if (myClass == 0) {
		JNIEnv *env = AndroidUtil::getEnv();
		jclass ref = env->FindClass(myName.c_str());
		myClass = (jclass)env->NewGlobalRef(ref);
		env->DeleteLocalRef(ref);
	}
	return myClass;
}

Member::Member(const JavaClass &cls) : myClass(cls) {
	//ZLLogger::Instance().registerClass(JNI_LOGGER_CLASS);
}

Member::~Member() {
}

Constructor::Constructor(const JavaClass &cls, const std::string &parameters) : Member(cls) {
	myId = AndroidUtil::getEnv()->GetMethodID(jClass(), "<init>", parameters.c_str());
}

jobject Constructor::call(...) {
	va_list lst;
	va_start(lst, this);
	jobject obj = AndroidUtil::getEnv()->NewObjectV(jClass(), myId, lst);
	va_end(lst);
	return obj;
}

Field::Field(const JavaClass &cls, const std::string &name, const JavaType &type) : Member(cls), myName(name) {
	myId = AndroidUtil::getEnv()->GetFieldID(jClass(), name.c_str(), type.code().c_str());
}

Field::~Field() {
}

Method::Method(const JavaClass &cls, const std::string &name, const JavaType &returnType, const std::string &parameters) : Member(cls), myName(name) {
	const std::string signature = parameters + returnType.code();
	myId = AndroidUtil::getEnv()->GetMethodID(jClass(), name.c_str(), signature.c_str());
}

Method::~Method() {
}

StaticMethod::StaticMethod(const JavaClass &cls, const std::string &name, const JavaType &returnType, const std::string &parameters) : Member(cls), myName(name) {
    std::string signature = parameters + returnType.code();
	myId = AndroidUtil::getEnv()->GetStaticMethodID(jClass(), name.c_str(), signature.c_str());
}

StaticMethod::~StaticMethod() {
}

ObjectField::ObjectField(const JavaClass &cls, const std::string &name, const JavaType &type) : Field(cls, name, type) {
}

jobject ObjectField::value(jobject obj) const {
	jobject val = AndroidUtil::getEnv()->GetObjectField(obj, myId);
	return val;
}

VoidMethod::VoidMethod(const JavaClass &cls, const std::string &name, const std::string &parameters) : Method(cls, name, JavaPrimitiveType::Void, parameters) {
}

void VoidMethod::call(jobject base, ...) {
	va_list lst;
	va_start(lst, base);
	AndroidUtil::getEnv()->CallVoidMethodV(base, myId, lst);
	va_end(lst);
}

IntMethod::IntMethod(const JavaClass &cls, const std::string &name, const std::string &parameters) : Method(cls, name, JavaPrimitiveType::Int, parameters) {
}

jint IntMethod::call(jobject base, ...) {
	va_list lst;
	va_start(lst, base);
	jint result = AndroidUtil::getEnv()->CallIntMethodV(base, myId, lst);
	va_end(lst);
	return result;
}

LongMethod::LongMethod(const JavaClass &cls, const std::string &name, const std::string &parameters) : Method(cls, name, JavaPrimitiveType::Long, parameters) {
}

jlong LongMethod::call(jobject base, ...) {
	va_list lst;
	va_start(lst, base);
	jlong result = AndroidUtil::getEnv()->CallLongMethodV(base, myId, lst);
	va_end(lst);
	return result;
}

BooleanMethod::BooleanMethod(const JavaClass &cls, const std::string &name, const std::string &parameters) : Method(cls, name, JavaPrimitiveType::Boolean, parameters) {
}

FloatMethod::FloatMethod(const JavaClass &cls, const std::string &name, const std::string &parameters) : Method(cls, name, JavaPrimitiveType::Float, parameters) {
}

jfloat FloatMethod::call(jobject base, ...) {
	va_list lst;
	va_start(lst, base);
	jfloat result = AndroidUtil::getEnv()->CallFloatMethodV(base, myId, lst);
	va_end(lst);
	return result;
}

jboolean BooleanMethod::call(jobject base, ...) {
	va_list lst;
	va_start(lst, base);
	jboolean result = AndroidUtil::getEnv()->CallBooleanMethodV(base, myId, lst);
	va_end(lst);
	return result;
}

static JavaPrimitiveType FakeString("Ljava/lang/String;");

StringMethod::StringMethod(const JavaClass &cls, const std::string &name, const std::string &parameters) : Method(cls, name, FakeString, parameters) {
}

jstring StringMethod::callForJavaString(jobject base, ...) {
	va_list lst;
	va_start(lst, base);
	jstring result = (jstring)AndroidUtil::getEnv()->CallObjectMethodV(base, myId, lst);
	va_end(lst);
	return result;
}

std::string StringMethod::callForCppString(jobject base, ...) {
	JNIEnv *env = AndroidUtil::getEnv();
	va_list lst;
	va_start(lst, base);
	jstring j = (jstring)env->CallObjectMethodV(base, myId, lst);
	va_end(lst);
	std::string str = AndroidUtil::fromJavaString(env, j);
	if (j != 0) {
		env->DeleteLocalRef(j);
	}
	return str;
}

ObjectMethod::ObjectMethod(const JavaClass &cls, const std::string &name, const JavaClass &returnType, const std::string &parameters) : Method(cls, name, returnType, parameters) {
}

jobject ObjectMethod::call(jobject base, ...) {
	va_list lst;
	va_start(lst, base);
	jobject result = AndroidUtil::getEnv()->CallObjectMethodV(base, myId, lst);
	va_end(lst);
	return result;
}

ObjectArrayMethod::ObjectArrayMethod(const JavaClass &cls, const std::string &name, const JavaArray &returnType, const std::string &parameters) : Method(cls, name, returnType, parameters) {
}

jobjectArray ObjectArrayMethod::call(jobject base, ...) {
	va_list lst;
	va_start(lst, base);
	jobjectArray result = (jobjectArray)AndroidUtil::getEnv()->CallObjectMethodV(base, myId, lst);
	va_end(lst);
	return result;
}

StaticObjectMethod::StaticObjectMethod(const JavaClass &cls, const std::string &name, const JavaClass &returnType, const std::string &parameters) : StaticMethod(cls, name, returnType, parameters) {
}

jobject StaticObjectMethod::call(...) {
	va_list lst;
	va_start(lst, this);
	jobject result = AndroidUtil::getEnv()->CallStaticObjectMethodV(jClass(), myId, lst);
	va_end(lst);
	return result;
}

StaticVoidMethod::StaticVoidMethod(const JavaClass &cls, const std::string &name, const std::string &parameters)
        : StaticMethod(cls, name,JavaPrimitiveType::Void, parameters) {
}

void StaticVoidMethod::call(...) {
    va_list lst;
    va_start(lst, this);
    AndroidUtil::getEnv()->CallStaticVoidMethodV(jClass(), myId, lst);
    va_end(lst);
}

StaticLongMethod::StaticLongMethod(const JavaClass &cls, const std::string &name, const std::string &parameters)
        : StaticMethod(cls, name,JavaPrimitiveType::Long, parameters) {
}

jlong StaticLongMethod::call(...) {
    va_list lst;
    va_start(lst, this);
    jlong result = AndroidUtil::getEnv()->CallStaticLongMethodV(jClass(), myId, lst);
    va_end(lst);

    return result;
}

