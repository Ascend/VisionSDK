/*
* -------------------------------------------------------------------------
*  This file is part of the Vision SDK project.
* Copyright (c) 2025 Huawei Technologies Co.,Ltd.
*
* Vision SDK is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*
*           http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
* -------------------------------------------------------------------------
 * Description: Pull the input video stream.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */
#include <string>
#include <glib.h>
#include <openssl/x509v3.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include "MxBase/Log/Log.h"
#include "MxPlugins/MxpiRtspSrc/RtspClientTlsInteraction.h"


// this struct should contain instance variable for the subclass
struct _RtspClientTlsInteraction {
    GTlsInteraction parent_instance;
    GTlsCertificate *caCert, *certKey;
    GTlsDatabase *database;
    gchar *crlPath;
};
G_DEFINE_TYPE(RtspClientTlsInteraction, rtsp_client_tls_interaction, G_TYPE_TLS_INTERACTION)

// this function handles all initialisation
static void rtsp_client_tls_interaction_init(RtspClientTlsInteraction *)
{
}

namespace  {
gchar* getCertificatePem(GTlsCertificate* cert)
{
    gchar *certificatePem;
    g_object_get(cert, "certificate-pem", &certificatePem, nullptr);
    return certificatePem;
}

X509* createOpensslCertificate(const gchar* pemData)
{
    BIO *bio = BIO_new_mem_buf((void*)pemData, -1);
    X509 *cert = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    return cert;
}

bool IsCertificateRevoked(X509* serverCert, X509* caCert, std::string crlPath)
{
    X509_STORE *store = X509_STORE_new();
    X509_LOOKUP *lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    X509_load_crl_file(lookup, crlPath.c_str(), X509_FILETYPE_PEM);
    X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK);
    X509_STORE_CTX *ctx = X509_STORE_CTX_new();
    X509_STORE_add_cert(store, caCert);
    X509_STORE_CTX_init(ctx, store, serverCert, nullptr);
    int result = X509_verify_cert(ctx);
    if (result != 1) {
        long errorCode = X509_STORE_CTX_get_error(ctx);
        const char* errorMsg = X509_verify_cert_error_string(errorCode);
        LogError << "Fail to verify peer cert according to the crl. " << std::string(errorMsg)
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    X509_STORE_CTX_free(ctx);
    X509_STORE_free(store);
    X509_LOOKUP_free(lookup);
    X509_free(serverCert);
    X509_free(caCert);
    return result != 1;
}

gboolean AcceptTlsCertificate(GTlsConnection *conn, GTlsCertificate *peerCert, GTlsCertificateFlags,
    RtspClientTlsInteraction* user_data)
{
    GTlsDatabase* database = g_tls_connection_get_database(G_TLS_CONNECTION(conn));
    if (database) {
        LogDebug << "tls peer certificate not accepted, checking user database ...";
        GSocketConnectable *peerIdentity = NULL;
        GError *error = NULL;
        (void)g_tls_database_verify_chain(database,
            peerCert,
            G_TLS_DATABASE_PURPOSE_AUTHENTICATE_SERVER,
            peerIdentity,
            g_tls_connection_get_interaction(conn),
            G_TLS_DATABASE_VERIFY_NONE,
            NULL,
            &error);
        if (error != 0) {
            LogError << "Failure verifying certificate chain: " << error->message << GetErrorInfo(APP_ERR_COMM_FAILURE);
            g_clear_error(&error);
            return FALSE;
        }
    }

    std::string crlPath = std::string(user_data->crlPath);
    // check peer certificated according to the ca and crl
    if (crlPath != "") {
        LogDebug << "check peer certificate and crl";
        auto certInPemForm = getCertificatePem(peerCert);
        X509* certInOpenSSLForm = createOpensslCertificate(certInPemForm);
        if (certInOpenSSLForm == nullptr) {
            LogError << "Create cert failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return FALSE;
        }
        auto caCertInPemForm = getCertificatePem(user_data->caCert);
        X509* caCertInOpenSSLForm = createOpensslCertificate(caCertInPemForm);
        if (caCertInOpenSSLForm == nullptr) {
            LogError << "Create caCert failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            X509_free(certInOpenSSLForm);
            return FALSE;
        }
        if (IsCertificateRevoked(certInOpenSSLForm, caCertInOpenSSLForm, crlPath)) {
            LogError << "Fail to check peer certificate." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return FALSE;
        }
    }
    return TRUE;
}

GTlsInteractionResult rtsp_client_request_certificate(GTlsInteraction *interaction, GTlsConnection *connection,
    GTlsCertificateRequestFlags, GCancellable *, GError **)
{
    RtspClientTlsInteraction *stls = (RtspClientTlsInteraction *)interaction;
    LogDebug << "rtsp client request certificate";
    g_signal_connect(connection, "accept-certificate", G_CALLBACK(AcceptTlsCertificate), stls);
    g_tls_connection_set_certificate(connection, stls->certKey);
    return G_TLS_INTERACTION_HANDLED;
}
} /* namespace  */

// virtual funcs overrides, properties, signal defs here
static void rtsp_client_tls_interaction_class_init(RtspClientTlsInteractionClass *gClass)
{
    GTlsInteractionClass *objectClass = G_TLS_INTERACTION_CLASS(gClass);
    objectClass->request_certificate = rtsp_client_request_certificate;
}

RtspClientTlsInteraction* rtsp_client_tls_interaction_new(GTlsCertificate *certKey, GTlsCertificate *caCert,
    GTlsDatabase* database, gchar* crlPath)
{
    RtspClientTlsInteraction *interaction = (RtspClientTlsInteraction *)g_object_new(RTSP_CLIENT_TLS_INTERACTION_TYPE,
        NULL);
    interaction->certKey = certKey;
    interaction->caCert = caCert;
    interaction->database = database;
    interaction->crlPath = crlPath;
    return interaction;
}

void rtsp_client_tls_interaction_unref(RtspClientTlsInteraction *interaction)
{
    if (interaction) {
        g_object_unref(interaction->certKey);
        g_object_unref(interaction->caCert);
        g_object_unref(interaction->database);
        g_free(interaction->crlPath);
        interaction->certKey = nullptr;
        interaction->caCert = nullptr;
        interaction->database = nullptr;
        interaction->crlPath = nullptr;
        g_object_unref(interaction);
    }
}